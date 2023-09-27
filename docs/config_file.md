# Configuration file
**Find below the documentation for the webserv configuration file.**\
This documentation uses an augmented Backus-Naur Form ([BNF](https://fr.wikipedia.org/wiki/Augmented_Backus-Naur_Form)) notation.
## Directives

### autoindex:
Enables or disables the directory listing display.
```
autoindex = "autoindex" SP "on" | "off" ";"
Default: autoindex off;
Context: main, server, location
```

### client_max_body_size:
Set the maximum allowed size of the client request body.
If the size of a request exceed the maximum size, the server will respond with a 413 status code (Request Entity Too Large).
Setting a size of 0 disable the checking of client request body size.
```
client_max_body_size = "client_max_body_size" SP size ";"
Default: client_max_body_size 1m;
Context: main, server
```

### deny:
Deny HTTP methods (GET, POST, DELETE) received from a client to a location/server.
```
deny = "deny" SP "GET" | "POST" | "DELETE" ";"
Default: ——
Context: location
```

### error_page:
Define error pages to be sent in response when a client request result in an error.
```
error_page = "error_page" 1*( SP code ) SP uri ";" ; handled code: 300, 400 and 500
Default: ——
Context: main, server, location
```

### index:
Define files that will be used as an index when client request ended with a "/" character.
Files are checked in the specified order.
Files can be specified with an absolute path.
```
index = "index" 1*( SP file ) ";"
Default: index index.html;
Context: main, server, location
```

### listen:
Specifies the address and port on which the server will listen to.
```
listen = "listen" SP port | ( address [ ":" port ] )";"
Default: *:80
Context: server
```

### location:
Set the configuration for a requested URI.
```
location =  "location" SP uri SP "{" 1*( LF *( HT | SP ) <rules with location in context> ) LF *HT "}"
Default: ——
Context: server
```

### root:
Set the root directory for requests.
Example, with the following configuration file:
```
    location /foo/ {
        root /bar
    }
```
The file `"/bar/foo/this.txt"` will be sent in response to the `"/foo/this.txt"` request.
```
root = "root" SP path ";"
Default: root html;
Context: main, server, location
```

### server:
Set the configuration for a virtual server.
```
server = "server" SP "{" 1*( LF *( HT | SP ) <rules with server in context> ) LF "}"
Default: ——
Context: main
```

### server_name:
Set the names of a virtual server.
The first name specified becomes the primary server name.
```
server_name = "server_name" 1*( SP hostname | address ) ";"
Default: ""
Context: server
```

### type:
Maps file name extensions to MIME types of responses.
```
type = "type" SP mime 1*( SP extension ) ";"
Default: ""
Context: main, server, location
```

### Index
```
address     = 1*3DIGIT 3 ( "." 1*3DIGIT ) ;each fragment must be between 0 and 255
code        = 3DIGIT ;must be between 300 and 599
extension   = 1*TEXT
file        = 1*TEXT
hostname    = *TEXT
mime        = 1*TEXT
path        = 1*TEXT *( "/" 1*TEXT )
port        = 1*DIGIT ; must be under uint16_t max
size        = 1*DIGIT [ "k" | "m" ]
uri         = *TEXT
```