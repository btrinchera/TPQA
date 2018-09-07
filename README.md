![TracePQM](./img/logo_TracePQM.png)


# TPQA-Traceable Power & Power Quality Analyzer


TPQA is being developed in the framework of [EMPIR](https://msu.euramet.org/calls.html) project [TracePQM](http://tracepqm.cmi.cz/)
and looks like as an open software tool suitable for handling of high performance and precise state-of-the-art digitizing system identified for
building of metrology grade measurement system to ensure reliable and traceable measurement of Power and Power Quality (PQ) complex parameters. Its use is not
only restricted to power and energy area, but it can be used in experiments which require simultaneous measurement of complex voltage and current
waveforms.

It has been developed using an ANSI c development environment such as [LabWindows/CVI](http://www.ni.com/lwcvi/whatis/hardware/), and  its  modular
structure is based on the following components:

 - Guide user interface (GUI) that hosts a control and data acquisition module optimized to operate with different hardware platforms.
 - Data processing module that allows to interact with the calculation scripts using the [Matlab](https://uk.mathworks.com/products/matlab.html) engine for off-line computation or through the [Advanced Analysis Libraries](http://zone.ni.com/reference/en-XX/help/370051AC-01/cvi/libref/cviadvanced_analysis_library_functi/) of the development environment for
on-line computation of some power and PQ electrical parameters.

TPQA handles the following digitizers using suitable drivers as:

 - [niScope](http://sine.ni.com/nips/cds/view/p/lang/cs/nid/12638) drivers for control of single or multiple PXI-5922 24-bit flexible resolution digitizer whose bandwidth falls within the medium frequency (MF) of radio frequency band.
 - Synchronized [HP/Agilent/Keysight3458A](https://www.keysight.com/en/pd-1000001297%3Aepsg%3Apro-pn-3458A/digital-multimeter-8-digit?cc=US&lc=eng) configured as  digitizer by making DC voltage measurements, by DCV digitizing, by direct-sampling, or by sub-sampling.

The off-line calculation of elecytrical Power and PQ parameters is performed by the [QWTB](https://qwtb.github.io/qwtb/)
toolbox. A list of supported algorithms can be found in the [report A2.4.4 - Algorithms Description](https://github.com/smaslan/TWM/blob/master/doc/A244%20Algorithms%20description.pdf).

The project is still under development. The current version includes the guide user interfaces (GUIs) trought which one can beging to gain experience with high performnce digitizers and waveform sampling. The follwing actions csan be performed:

- In the present release it is possibile to perform single shot or continous acquisition with measurement system equiped with two PXI-5922 boards configured for pseudo-differential measurements for pure and distorted waveforms. The part related to DMMs 3458A is still under development.
<img src="./img/GUI_HW_Control.png">

- Off-line data processing (only for TPQA release under LabWindows/CVI environment) using the Matlab to CVI interface based on [TWM](https://github.com/smaslan/TWM) modularity concept. 
<img src="./img/GUI_Matlab_CVI.png">

- On-line processing of recorded data and analysis in the frequency domain using the Advanced Analysis Library provided by LabWindows/CVI. 
<img src="./img/GUI_2.png">


## Download

Here you can download build only:

## License
The TPQA is distributed under [MIT license](./TPQA/LICENSE.md/LICENSE.md). Note the algorithms in the QWTB toolbox may have different licenses.


