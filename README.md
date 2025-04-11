# Mosaify

## VERSION
v1.0.3 - Exposed libraries and headers.

## Create Release
```bash
 VERSION=v1.0.3
 git add .
 git commit -m "Release ${VERSION}"
 git push
 git tag -a ${VERSION} -m "Release ${VERSION}"
 git push origin ${VERSION}
```

## Table of Contents

 - [About](#about)
 - [Getting Started](#getting_started)
 - [Installing](#installing)
 - [Usage](#usage)
 - [Contributing](#contributing)

## About
Could you write about 1-2 paragraphs describing the purpose of your project? 

Might use this for creating a tiled image: https://legacy.imagemagick.org/Usage/montage/


## Getting Started
These instructions will get you a copy of the project up and running on your local machine for development and testing.

### Third party libraries
* [GLM - 0.9.3.2](https://github.com/icaven/glm/tree/0.9.3.2)
* [nlohman::json - v3.11.3](https://github.com/nlohmann/json/tree/v3.11.3)
* [libboard](https://github.com/c-koi/libboard)
### Packages
 * `sudo apt install python3.8-dev`
 * `sudo apt install libmagick++-dev`

1. Clone Thirdpary library, this project assumes they are located in the same directory as the project.
1. In Terminal, `cd` to the root of this project.
1. Symbolically link the thirdparty lib in the root of the project. (`ln -s ./third/party/lib/dir thirdpartylink`)
    *  For GLM the `thirdpartylink` is `glm`
    *  For nloghman::json the `thirdpartylink` is `json`

1. Fork/Clone

1. Create the virtual environment:

    ```sh
    # TODO
    ```

1. Activate the virtual environment:

    ```sh
    # TODO
    ```

## Installing
Installation instructions.

1. Install the requirements:

    ```sh
    # TODO
    ```
    
## Usage
A step-by-step series of examples that tell you how to get a development env running.
1. Run

    ```sh
    # TODO
    ```

## Contributing
Contributors names.
