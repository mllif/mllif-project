# Create C++ Wrapper From Symbols

1. Prepare MSM file
2. Pipe to `mllif-cxx`
3. Use generated header file!

All MLLIF Backends use stdin as input and stdout as output.
So, You should pipe msm file to process:

```sh
mllif-cxx < my.msm > wrapper.h
```

-or-

```sh
cat my.msm | mllif-cxx > wrapper.h
```

Both scripts do same thing!
You can choose one by your preference.
