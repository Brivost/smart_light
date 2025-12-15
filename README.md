Added Sparkfun's version of AmbiqSuiteSDK as submodule:
```zsh
git submodule add https://github.com/sparkfun/AmbiqSuiteSDK.git external/AmbiqSuiteSDK

git submodule update --init --recursive
```

Successfully uploaded firmware using these commands:

```zsh
cd external/AmbiqSuitSDK/boards_sfe/common/examples/blinky/gcc

make BOARD=redboard_artemis_atp COM_PORT=/dev/cu.usbserial-110 COM_PORT=/dev/cu.usbserial-110 SVL_UPLOAD_BAUD=115200 bootload
```