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

To run the classical, non-optimized, promethee implementation, use th `-V` (Vanilla) flag
```
./run -V path/to/input/ path/to/meta/ path/to/output/
```

## Input format

The input data should be specified in two directories; one describes the evaluation of the alternatives to each considered criterion, the other describes the preference function to compare alternatives. 

In the alternatives directory, the user should add a file to each criterion, following the `NAME_OF_CRITERION.input` convention. These files indicate the evaluation of each alternative according to the criterion specified, in a matrix format, separated by an empty speace and new lines separator. 

The second directory, which specifies the preference functions, should have a file to each criterion, using the `NAME_OF_CRITERION.meta` convention. Each file should follow the pattern:
```
weight
function_name
parameters
is_max
```
In which `weight` is the value of weight of the criterion specified, `function_name` is the name of the function, `parameters` are the parameters, separated by an empty space, for the specific function, and `is_max indicates` if in the comparison greater values are desired over the smaller ones (0 if smaller values are desired, 1 for greater values).

Example:
```
0.2
level
0.5 1
1
```

We support all the prefence functions described in the literature. See below how each one of them should be declared in the `function_name` parameter:

```
Usual => usual
Quasi (U-shape) => quasi
Linear (V-shape) => linear
Level => level
Linear with Indifference => linearWithIndifference
Gaussian => gaussian
```

For the optimized version, we support the Linear shape (as the proposed method is restricted to Linear functions). Note that, however our implementation only supports linear comparisson, one can use the parameter `p = 0` and linear comparisson will behave usual comparisson.

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

## References

[PROMETHEE is not quadratic: An O(qnlog(n)) algorithm](https://www.sciencedirect.com/science/article/pii/S0305048317303729)<br>
[A Preference Ranking Organisation Method: The PROMETHEE Method for Multiple Criteria Decision-Making](https://www.jstor.org/stable/2631441)
