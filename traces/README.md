## Power Trace Measurements

The following power traces were recorded, and will be made available.

### CW-Lite External Measurements

- 500 000 traces of key0, random plaintext: https://powertraces.sfo2.digitaloceanspaces.com/saml11_paper/saml11/cwlite/cwlite_ext_500k_key0_randtext.cwp
- 500 000 traces of key1, random plaintext: https://powertraces.sfo2.digitaloceanspaces.com/saml11_paper/saml11/cwlite/cwlite_ext_500k_key1_randtext.cwp
- 500 000 traces of random key, random plaintext: https://powertraces.sfo2.digitaloceanspaces.com/saml11_paper/saml11/cwlite/cwlite_ext_500k_randkey_randtext.cwp

#### ChipWhisperer Project File Traces

These three files are part of a "ChipWhisperer project file". You will need to decode the project file if you want to download all traces associated with that file.

For example, to get all traces you would see the CWP file at:

https://powertraces.sfo2.digitaloceanspaces.com/saml11_paper/saml11/cwlite/cwlite_ext_500k_key0_randtext.cwp

This contains a .cfg file for each section, which the first one is located at:

https://powertraces.sfo2.digitaloceanspaces.com/saml11_paper/saml11/cwlite/cwlite_ext_500k_key0_randtext_data\traces\config_2019.01.06-08.53.24_.cfg

From that .cfg file you can see the "prefix". This prefix allows you to build the trace data files. In this example the prefix from the .cfg file is ```2019.01.06-08.53.24_```. Thus the data files would be:

https://powertraces.sfo2.digitaloceanspaces.com/saml11_paper/saml11/cwlite/cwlite_ext_500k_key0_randtext_data/traces/2019.01.06-08.53.24_textin.npy

https://powertraces.sfo2.digitaloceanspaces.com/saml11_paper/saml11/cwlite/cwlite_ext_500k_key0_randtext_data/traces/2019.01.06-08.53.24_textout.npy

https://powertraces.sfo2.digitaloceanspaces.com/saml11_paper/saml11/cwlite/cwlite_ext_500k_key0_randtext_data/traces/2019.01.06-08.53.24_traces.npy

https://powertraces.sfo2.digitaloceanspaces.com/saml11_paper/saml11/cwlite/cwlite_ext_500k_key0_randtext_data/traces/2019.01.06-08.53.24_keylist.npy

### Internal ADC with Op-Amp (board 'A')
- dly = 19 for these traces:
- 8 000 000 traces of key0, random plaintext

#### Additional Delay Test Measurements
- dly = 10 for these traces:
- dly = 11 for these traces:
- dly = 12 for these traces:
- dly = 13 for these traces:
- dly = 14 for these traces:
- dly = 15 for these traces:
- dly = 16 for these traces:
- dly = 17 for these traces:
- dly = 18 for these traces:

#### URL For Board A ####

All of the 'board a' measurements are of the following form, where you can adjust the 'dly19' to reflect the correct phase offset.

https://powertraces.sfo2.digitaloceanspaces.com/saml11_paper/saml11/testboards/boarda_dly19_shunt_textin_array_0.npy

https://powertraces.sfo2.digitaloceanspaces.com/saml11_paper/saml11/testboards/boarda_dly19_shunt_trace_array_0.npy

https://powertraces.sfo2.digitaloceanspaces.com/saml11_paper/saml11/testboards/boarda_dly19_shunt_textout_array_0.npy

### Internal ADC with Op-Amp (board 'A'), Secure to Non-Secure

https://powertraces.sfo2.digitaloceanspaces.com/saml11_paper/saml11/testboards/boarda_trustzone_s_ns_shunt_textin_array_0.npy

https://powertraces.sfo2.digitaloceanspaces.com/saml11_paper/saml11/testboards/boarda_trustzone_s_ns_shunt_textout_array_0.npy

https://powertraces.sfo2.digitaloceanspaces.com/saml11_paper/saml11/testboards/boarda_trustzone_s_ns_shunt_trace_array_0.npy

### Internal ADC with shunt, VCC_Core/4 as ADC Source (board 'B')
- dly = 18 for these traces:
- 32 000 000 traces of key0, random plaintext

These traces are held as 20 sets of files. The following URLs could be used with WGET to retrieve the entire set. Note that the `b' board doesn't need the full trace set to break the board.

https://powertraces.sfo2.digitaloceanspaces.com/saml11_paper/saml11/testboards/boardb_shunt_textout_array_{0..19}.npy

https://powertraces.sfo2.digitaloceanspaces.com/saml11_paper/saml11/testboards/boardb_shunt_textin_array_{0..19}.npy

https://powertraces.sfo2.digitaloceanspaces.com/saml11_paper/saml11/testboards/boardb_shunt_trace_array_{0..19}.npy

### Development Kit - Linear Regulator
- dly = 18 for these traces:
- 240 000 000 traces of key0, random plaintext
- TODO: 240 000 000 traces of key1, random plaintext

https://powertraces.sfo2.digitaloceanspaces.com/saml11_paper/saml11/testboards/no_shunt_extended_textout_array_{0..149}.npy

https://powertraces.sfo2.digitaloceanspaces.com/saml11_paper/saml11/testboards/no_shunt_extended_textin_array_{0..149}.npy

https://powertraces.sfo2.digitaloceanspaces.com/saml11_paper/saml11/testboards/no_shunt_extended_traces_array_{0..149}.npy

### Development Kit - Switching Regulator
- dly = 18 for these traces:
- 240 000 000 traces of key0, random plaintext
- TODO: 240 000 000 traces of key1, random plaintext


https://powertraces.sfo2.digitaloceanspaces.com/saml11_paper/saml11/testboards/no_shunt_switching_textout_array_{0..149}.npy

https://powertraces.sfo2.digitaloceanspaces.com/saml11_paper/saml11/testboards/no_shunt_switching_textin_array_{0..149}.npy

https://powertraces.sfo2.digitaloceanspaces.com/saml11_paper/saml11/testboards/no_shunt_switching_traces_array_{0..149}.npy
