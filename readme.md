# ArtNet Node

## Web Interface

### pre-requisites to build `preact` web app

* NodeJS
* Yarn

### build web app and prepare SPIFFS image

`> cd www`

`> yarn install`

`> yarn build`

this will:

* build the web app in www/src
* minify and gzip all the files
* copy them to /data/www so they are ready to be uploaded to SPIFFS

## Platformio on Linux
`sudo usermod -a -G dialout $USER`

and log out / log in