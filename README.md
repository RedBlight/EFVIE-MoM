# EFVIE-MoM

Implemented a method of moment solver to the electric field volume integral equation for electromagnetic scattering, using SWG basis functions.

![Electric Field Volume Integral Equation](EFVIE.png "Electric Field Volume Integral Equation1")

Unlike my SBR-PO implementation, this is a full wave solution and it makes no simplifying assumptions about the electromagnetic phenomena. As such, this is mathematically and computationally more intensive compared to it. This is also more computationally intensive than surface integral equation solvers, but it makes us able to analyze scattering from non-homogeneous objects.

Here is how computationally intensive this is: when you increase the frequency of the incident wave 2 times, the amount of tetrahedrons in the object model has to increase 2^3 times, and so the MoM matrix grows (2^3)^2 times, which we have to solve in order to get the results. It's very easy to get out of time and memory bounds as the object in analysis gets electrically large.

I attempted to do this in a 4-week internship at GTU. Although I have completed every stage of it, there are some bugs in it's current state and will fix them in a later time.

See my internship diary (in Turkish) for more information.
