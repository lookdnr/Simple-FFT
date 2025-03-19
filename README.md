# C++ Coursework: Seismogram Processing Using the Cooley-Tukey FFT Algorithm and Numerical Integration

**Author**: Luke Dinsdale (02205881)

**Date**: March 2025

---

I have written a program that reads single-component seismograms from a file and uses an implementation of the Fast Fourier Transform (FFT) algorithm by Cooley and Tukey (1965) alongside numerical integration to output the spectra of acceleration, velocity, and displacement, as well as the corresponding time-domain records. I’ve always wondered how the Fourier Transform (FT) was computed discretely, and reading about the importance of this algorithm for modern signal processing—and life as a whole—led me to attempt its implementation. This was a great opportunity to become more acquainted with C-style programming in a practical sense and explore an important algorithm.

---

## Background

The Discrete Fourier Transform (DFT) of $N$ data points can be written as:

$$ F[k] = \sum_{n=0}^{N-1} t_n e^{-i 2\pi \frac{k}{N} n}, \quad k = 0, 1, \dots, N-1,$$

where $t_n$ is the signal value at the $n$-th time sample, and $F[k]$ is the complex amplitude at the $k$-th frequency bin. The frequency of the $k$-th bin is $f_k = \frac{k f_s}{N}$, where $$f_s$$ is the sampling frequency, and the twiddle factor is:

$$ e^{-i 2\pi \frac{k}{N} n} = \cos\left(2\pi \frac{k}{N} n\right) - i \sin\left(2\pi \frac{k}{N} n\right). $$

This direct implementation, often called the *naive* DFT, requires $N$ operations for each of the $k$ frequency bins, leading to a time complexity of $O(N^2)$, which is extremely inefficient for large $N$. The [Cooley-Tukey algorithm](https://en.wikipedia.org/wiki/Cooley%E2%80%93Tukey_FFT_algorithm), or *fast* FT, addresses this by splitting the summation into smaller sub-problems, considering the even ($t_{2r}$) and odd ($t_{2r+1}$) parts separately:

$$ F[k] = \sum_{r=0}^{\frac{N}{2}-1} t_{2r} e^{-i 2\pi \frac{r k}{N}} + e^{-i 2\pi \frac{k}{N}} \sum_{r=0}^{\frac{N}{2}-1} t_{2r+1} e^{-i 2\pi \frac{r k}{N}}, $$

where $\omega_N^k = e^{-i 2\pi \frac{k}{N}}$. This approach divides the $N$-point DFT into two $N/2$-point DFTs. If $N = 2^n$, $n \in \mathbb{N}$, the algorithm can recursively or iteratively divide a DFT problem into smaller sub-problems. By exploiting the symmetry and periodicity of the transform, it reduces the time complexity to $O(N \log N)$, providing a viable method for computing the Fourier Transform of discrete time-domain data for large $N$.

We can also leverage the frequency-domain integration property of the Fourier Transform:

$$ \mathscr{F} \left[ \int f(t) \, dt \right] = \frac{F(\omega)}{i \omega}. $$

This avoids the iterative approach required for time-domain integration, replacing it with simple algebraic operations, allowing efficient computation of velocity and displacement spectra from acceleration data.

**Note**: My implementation is heavily influenced by *Numerical Recipes: The Art of Scientific Computing* (Press et al., 2007, 3rd ed.) and *An Introduction to Numerical Methods in C++* (Flowers, 2000). I also used StackExchange methods for listing files within a directory and resetting the input failbit, contributed by [Parker (2009)](https://stackoverflow.com/a/8520560) and [Greyfade (2011)](https://stackoverflow.com/a/58738) on lines 38 and 15 in `CFileHandling.cpp` and `CUserInput.cpp`, respectively.

---

## User Guide

The program requires three inputs from the user: a file name from those listed in the terminal, a sampling frequency $f_s$, and a tapering constant to control windowing (see [Windowing](#windowing)). Error handling prevents crashes on invalid entries, but users must determine appropriate values for $f_s$ and the tapering constant.

### Example Usage

Results from example use cases are explored in the Jupyter notebook `Examples.ipynb`. Let’s walk through processing `Loma_Prieta.dat`:

When `main()` is called, the program lists files in the `/data/` directory and requests a file name:

`>> Hello!`

`>> Here are the files in \data:`

`"Kocaeli.dat"`

`"Loma_Prieta.dat"`

`"Northridge.dat"`

`"sine.txt"`

`>> Enter a file to proceed with:`

Enter `>> Loma_Prieta.dat` (not case-sensitive, but include the extension). Then:

`>> Enter a sampling frequency (Hz): `

Looking at `Loma_Prieta.dat`, we can see that the sampling frequency is 100 Hz, so enter `>> 100`. $f_s$ must be a number greater than zero. The algorithm ignores redundancies due to central symmetry, so the peak output frequency is the Nyquist frequency ($f_s / 2$) by design.

Next, the program provides information about the tapering constant and prompts:

`Enter a tapering constant k (put 1 if unsure): `


For this example, $k = 1$ was used, so enter `>> 1`. $k$ must be greater than zero.

The program processes the data, creates a results directory if needed, and exits upon completion.

---

## Classes and Functionality

The program is split into four classes:

- **`FileHandling`**: Handles reading and writing data.
- **`UserInput`**: Receives user-input parameters that modify the FFT and post-processing.
- **`FFT`**: Implements the FFT routine, including zero-padding.
- **`SignalProcessor`**: Manages post-processing (integration, windowing, amplitude computation).

Data are passed between classes using getters and setters to maintain encapsulation. Most data are handled with dynamically allocated arrays.

### Workflow

1. Two-column data (time, aceeleration) are read from a user-specified file into a vector and copied into an $N \times 1$ array.
2. A ampling frequency $f_s$ and tapering parameter $k$ are read from the user.
3. The data are transferred to an $N \times 2$ array of pointers, padded with zeros to length $2^n$. The first column is the real component, the second is imaginary (all zero for raw data).
4. The forward FFT is computed using the "divide and conquer" method: a bit reversal stage orders the data, followed by a butterfly operation stage recombining odd and even parts, with twiddle factors updated iteratively.
5. The data are integrated using the Fourier transform’s integration property, storing velocity and displacement spectra in new $N \times 2$ arrays.
6. The velocity and displacement spectra undergo an inverse FFT to obtain time-domain records, stored in new $N \times 2$ arrays.
7. Amplitudes of the complex data are computed and assigned to frequency or time increment bins.
8. The data are windowed (see [Windowing](#windowing)) to combat spectral leakage.
9. If no results directory exists, one is created. The spectra and time-domain outputs are written to labeled `.txt` files.

---

## References

- Cooley, J. W., & Tukey, J. W. (1965). An algorithm for the machine calculation of complex Fourier series. *Mathematics of Computation*, 19(90), 297-301.
- Press, W. H., Teukolsky, S. A., Vetterling, W. T., & Flannery, B. P. (2007). *Numerical Recipes: The Art of Scientific Computing* (3rd ed.). Cambridge University Press.
- Flowers, B. H. (2000). *An Introduction to Numerical Methods in C++*. Oxford University Press.
- Parker, J. (2009). Answer to "How to list files in a directory in C++". *Stack Overflow*. [https://stackoverflow.com/a/8520560](https://stackoverflow.com/a/8520560)
- Greyfade (2011). Answer to "How to reset cin when using it?". *Stack Overflow*. [https://stackoverflow.com/a/58738](https://stackoverflow.com/a/58738)

---

## Appendix

### General

#### Windowing

The routine applies a [Hann window](https://en.wikipedia.org/wiki/Hann_function) to combat spectral leakage from resampling to $N = 2^n$ and to mitigate extreme values from dividing by small $\omega$ in integration. The function `hannWindow()` implements:

$$ f_n(\bullet) = f_n(\bullet) \cdot k \cdot 0.5 \left(1 - \cos\left(\frac{2\pi n}{N}\right)\right), $$

where $k$ is a user-input tapering constant, $k \in \mathbb{R}, k \geq 0$. When $k = 1$, it’s the standard Hann window; $0 \leq k < 1$ reduces tapering, and $k > 1$ increases it linearly (e.g., $k = 2$ doubles the effect).

#### File Formatting

The input file format is strict: two tab-delimited columns (time and acceleration) with five header lines of metadata. For example, `sine.txt`:
