## About
This repository contains the firmware for the ESP8266 microcontroller that is the 'big brain' in the [U:Kit sensor kit](https://github.com/attachix/ukit) project.

## Software
You will need an ESP8266 toolchain and SDK. Probably the easiest way to set one is to use a [docker container](https://github.com/attachix/docker-c9-esp8266-sdk).
Optionally you may need `npm` and `nodejs` to work with the files in the `web` folder.

## Features
* Over The Air Updates (OTA):
To enable it make sure the ENABLE_OTA=1 directive is present during compile time.

* JavaScript VM (JSVM):
Use ENABLE_JSVM=1 to activate it. Example

## Host Emulator

* Allow sending and receiving bytes without buffering:

    stty -icanon && stdbuf -i0 -o0 nc localhost 10000

## NodeJs Stuff

* Trouble-shooting
If there is an issue with the installation try the following steps:

    rm -rf node_modules
    rm -rf package-lock.json
    npm cache clean
    npm install

## Emscripten

Up to now we have good results with creating a pure JavaScript code using the latest sdk.
There is a docker container that creates the needed environment.
It can be built and run  with following commands

    docker build -f components/jerryscript/Dockerfile -t jerryscript-ems .
    docker run -it jerryscript-ems

Inside the container a new jerryscript snapshot compiler can be built by calling

    node /jerryscript/build/bin/jsc.js -o /tmp/test.js.snap /tmp/test.js

Where `test.js.snap` is the output file and /tmp/test.js is an existing JavaScript file.

To update the already precompiled jsc.js in `web/dev` you can do the following:

    docker run -v $(pwd)/web/:/web -it jerryscript-ems

And run the following commands inside the container

    cp -r /jerryscript/build/bin/jsc.* /web/dev

That will require also precompilation of the main.js file.
Inside the container this can be done with the following command.

    node /jerryscript/build/bin/jsc.js -o /web/build/main.js.snap /web/dev/main.js



## Copyright
Copyright (c) 2016-2020, Slavey Karadzhov <slav@attachix.com>. All rights reserved.

## Authors
* Slavey Karadzhov <slav@attachix.com>
* .. Contributors - See https://github.com/attachix/ukit-firmware/graphs/contributors

## License
All files in this repository are licensed under [GPLv3](LICENSE) except:
* JerryScript - under lib/jerryscript that is licensed under [Apache License, Version 2.0](./lib/jerryscript/LICENSE).
* Sming - under lib/Sming that is licensed under LGPL.
