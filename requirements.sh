## Basic defines pre-configured from user
maxfps=10
block_size_array=(2 4 8 16)
PYTHON_EXECUTABLE=C:/Users/gamin/AppData/Local/Programs/Python/Python39/python.exe

## Automatically installs required Python3 packages
function install_packages() 
{
    if [ -f "requirements.txt" ]; then
        # Read the packages from requirements.txt
        packages=$(<requirements.txt)

        # Loop through each package and check/install
        for package in $packages; do
            if ! command -v "$package" &> /dev/null; then
                echo "Installing $package..."
                pip install "$package"
                echo "Installed $package."
            else
                echo "$package is already installed."
            fi
        done
    else
        echo "requirements.txt not found."
        exit 1
    fi
}