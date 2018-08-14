# PrometheeOptimization

Implementation of Promethee with optimization O(qnlog(n)) following [this](https://www.sciencedirect.com/science/article/pii/S0305048317303729) article.

## Performance Plot O(qnlog(n)) x O(qn²)

![Optimization x Vanilla Plot](assets/Rplot.png?raw=true "Performance Plot")<br>

red: O(qn²) [Our implementation](https://github.com/simsab-ufcg/promethee-vanilla)<br>
green: O(qnlog(n)) This implementation


## Accepted format

You should use 2 folders to represent criterions input/ and meta/. <br><br>
Input folder must be filled with files that represent critetion value for each alternative, this file following this pattern: <br>
NAME_OF_CRITERION --- the data of every alternative in that specific criterion in matrix format.<br><br>
Meta folder must de filled with parameters specification for each preference function that will be used in each criterion, this files must have the same name of the respective criterion in input folder.<br>

### Function and parameters specification

Each file in meta folder should following this pattern:

```
weight
function_name
parameters
is_max
```
In which weight is the value of weight that criterion,function_name is the name of the functions (currently is only implemented linear comparisson), parameters are the parameters for the specific function and is_max indicates if in the comparison greater values are desired over the smaller ones (0 if smaller values are desired, 1 for greater values).

Example:
```
0.2
linear
1
1
```

## Available preference functions

Our implementation only supports linear comparisson, but can be used parameter = 0 and linear comparisson will behave usual comparisson.

## How to run
Execute the following commands in terminal
```
make
./run path/to/input path/to/meta path/to/output
```
The result will be put in your path/to/output.

## References

[PROMETHEE is not quadratic: An O(qnlog(n)) algorithm](https://www.sciencedirect.com/science/article/pii/S0305048317303729)<br>
[A Preference Ranking Organisation Method: The PROMETHEE Method for Multiple Criteria Decision-Making](https://www.jstor.org/stable/2631441)
