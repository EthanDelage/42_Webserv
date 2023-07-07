# Configuration file
**Find below the documentation for the webserv configuration file.**\
This documentation uses an augmented Backus-Naur Form ([BNF](https://fr.wikipedia.org/wiki/Augmented_Backus-Naur_Form)) notation.
## Directives

### autoindex:
```
autoindex = "autoindex" SP "on" | "off" ";"
Default: autoindex off;
Context: main, server, location
```

### client_max_body_size:
```
client_max_body_size = "client_max_body_size" SP size ";"
Default: client_max_body_size 1m;
Context: main, server
```

### deny:
```
deny = "deny" SP "GET" | "POST" | "DELETE" ";"
Default: ——
Context: server, location
```

### error_page:
```
error_page = "error_page" 1*( SP code ) SP uri ";"
Default: ——
Context: server
```

### index:
```
index = "index" 1*( SP file ) ";"
Default: index index.html;
Context: main, server, location
```

### listen:
Specifies the address and port on which the server will listen to.
```
listen = "listen" SP port | ( address [ ":" port ] ) [ SP "default" ] ";"
Default: *:80
Context: server
```

### location:
```
location =  "location" SP uri SP "{" LF 1*<rules with location in context> LF "}"
Default: ——
Context: server
```

### root:
```
root = "root" SP path ";"
Default: root html;
Context: main, ,server, location
```

### server:
```
server = "server" SP {" LF 1*<rules with server in context> LF "}"
Default: ——
Context: main
```

### server_name:
```
server_name = "server_name" 1*( SP hostname | address ) ";"
Default: ""
Context: server
```

### Index
```
address     = 1*3DIGIT 3 ( "." 1*3DIGIT )
file        = 1*TEXT
hostname    = *TEXT
path        = 1*TEXT *( "/" 1*TEXT )
port        = 1*DIGIT
size        = 1*DIGIT [ "k" | "m" ]
uri         = 1*TEXT
```