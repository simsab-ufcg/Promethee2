# PrometheeOptimization

An implementation of Promethee 2 based on the "PROMETHEE is not quadratic: An O(qnlog(n)) algorithm" [paper](https://www.sciencedirect.com/science/article/pii/S0305048317303729) by Toon Calders and Dimitri Van Assch.

# Building/Running

## Build
To build our project just run ```make``` command in the root directory.

## Run
You can use main.py to manager all your Promethee runs. First, must be specified Promethee version, Fast Promethee is our more stable and fast version, -fp is his flag. To use Umbu version -um must be used and to Vanilla version -van.

Fast Promethee and Umbu use tiff input format, while Vanilla uses txt input format.  

## Input format

### Tiff Format
You must process each criterion separated and finally merge everyone.
Umbu and Fast Promethee share some flags to must be used, like as:
- Raster name
- Weight of criteria
- Preference functions and preference functions parameter
- Flag to define if criteria are directly proportional to values, otherwise not use this flag.
Sample as:
```
python main.py -fp AEMMF.tif 0.5 -type=linear 1 -ismax
```
But Fast Promethee has two optionals flags:
- The number of the process which can be used to calculate promethee (Default: 1).
- Define the max sum MB of RAM used in the all process which will run the algorithm(Default: 1024).

```
python main.py -fp AEMMF.tif 0.5 -type=linear 1 -size=2048 -proc=4
```

Umbu has one optional flag:
- The number of alternatives which will be saved in one chunk.
```
python main.py -um AEMMF.tif 0.5 -chunk=100000000 -type=linearWithIndifference 1 0.5
```

Vanilla supports:

Usual => usual
Quasi (U-shape) => quasi
Linear (V-shape) => linear
Level => level
Linear with Indifference => linearWithIndifference
Gaussian => gaussian

For the optimized version, we support the Linear shape (as the proposed method is restricted to Linear functions). Note that, however, our implementation only supports linear comparison, one can use the parameter `p = 0` and linear comparison will behave usual comparison.

Umbu and Fast Promethee only support linear and linearWithIndifference functions.

#### Out of Zone of Study

You can use Not-a-Number instead of value of alternative, this alternatives will not be considered. This can be useful if you need run Promethee to calculate a subregion of Tiff file.

## References

[PROMETHEE is not quadratic: An O(qnlog(n)) algorithm](https://www.sciencedirect.com/science/article/pii/S0305048317303729)<br>
[A Preference Ranking Organisation Method: The PROMETHEE Method for Multiple Criteria Decision-Making](https://www.jstor.org/stable/2631441)
