# `modules/req` - simple web request library

This library is dedicated to GET/POST requests

## Building



## Exported Types

`req.Result` -> simple object describing an HTTP response, including the URL it was made with (`.url`), the return text/data (`.text`), and the status code integer (`.status_code`)

## Exported Functions

`req.GET(url, data=none)` -> return a Result object describing an HTTP GET request to a given URL, with `data` being a dictionary of settings

`req.POST(url, data=none)` -> return a Result object describing an HTTP POST request to a given URL, with `data` being a dictionary of settings

`req.download(url, dest, data=none)` -> return a Result object describing an HTTP GET request to a given URL, downloading to a file name `dest`. `dest` can also be an already open iostream, in which case it will append it to the file


