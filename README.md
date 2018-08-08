# PrometheeOptimization
Implementation of Promethee with optimization

## Accepted format

You should use the following pattern in files names for every criterion: <br>
NAME --- the data of every alternative in that specific criterion <br>
NAME_p --- the specification of the function and parameters, though this file is not obligatory (If it's not specified, default values are going to be used)

### Function and parameters specification

The NAME_p content should be in the following format:

```
function_name
parameters
is_max
```
In which function_name is the name of the functions (currently is only implemented linear comparisson), parameters are the parameters for the specific function and is_max indicates if in the comparison greater values are desired over the smaller ones (0 if smaller values are desired, 1 for greater values).

Example:
```
linear
1
1
```

This example corresponds to the default values of the file.


## How to run
Put your files in promethee/input and execute the following commands in terminal
```
chmod +x run
./run name_file1 weight1 name_file2 weight2 ...
```
The result will be put in promethee/output
