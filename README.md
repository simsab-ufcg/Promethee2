# PrometheeOptimization

An implementation of Promethee 2 based on the "PROMETHEE is not quadratic: An O(qnlog(n)) algorithm" [paper](https://www.sciencedirect.com/science/article/pii/S0305048317303729) by Toon Calders and Dimitri Van Assch.

# Building/Running

## Build
To build our project just run ```make``` command in the root directory.

## Run
To run an promethee execution, execute the following command
```
./run path/to/input/ path/to/meta/ path/to/output/
```
The `path/to/input/` and `path/to/meta/` directories keep the specification of alternatives and criteria to analysed. Please, see the example in the `samples` directory. The input specs are described in the following section. The output of the promethee execution will be storage in the `path/to/output` directory.

To run the classical, non-optimized, promethee implementation, use th `-V` flag
```
./run -V path/to/input/ path/to/meta/ path/to/output/
```

## Input format

You should use 2 folders to represent criterions input/ and meta/. <br><br>
Input folder must be filled with files that represent a alternative value for each criterion, this file following this pattern: <br>
NAME_OF_CRITERION.input --- the data of every alternative in that specific criterion in matrix format.<br>
All files must be matrix with same size.<br><br>
Meta folder must be filled with parameters specification for each preference function that will be used in each criterion, this files must have the same name of the respective criterion in input folder but .meta extension instead of .input.<br>

### Function and parameters specification

Each file in meta folder should following this pattern:

```
weight
function_name
parameters
is_max
```
In which weight is the value of weight that criterion, function_name is the name of the functions, parameters are the parameters for the specific function and is_max indicates if in the comparison greater values are desired over the smaller ones (0 if smaller values are desired, 1 for greater values).

Example:
```
0.2
level
0.5 1
1
```
Functions which use more of than 2 parameters must be follow pattern _p_, _q_.

### Out of Zone of Study

In cases where not all of the alternatives must be counted to calculate the Flow, will should use "nan" (case sensitive) in criterion value to determine that this alternative will not be used.<br>
This is useful when the input matrix is an image.<br><br>
Example:
```
nan 2.0 nan
3.5 4.8 6.5
nan nan 5.7
```
The alternative is determined by row and colunm in matrix, so "nan" must be in the same positions to all criterion matrix.

## Available preference functions

#### Vanilla

```
Usual => usual
Quasi (U-shape) => quasi
Linear (V-shape) => linear
Level => level
Linear with Indifference => linearWithIndifference
Gaussian => gaussian
```

#### Optimization

```
Linear (V-shape) => linear
```

Our implementation only supports linear comparisson, but can be used parameter _p_ = 0 and linear comparisson will behave usual comparisson.

## References

[PROMETHEE is not quadratic: An O(qnlog(n)) algorithm](https://www.sciencedirect.com/science/article/pii/S0305048317303729)<br>
[A Preference Ranking Organisation Method: The PROMETHEE Method for Multiple Criteria Decision-Making](https://www.jstor.org/stable/2631441)
