# python_c_class

## build

```bash
python setup.py build_ext --inplace

```

Usage
```python
from creader import ClusterFileReader


r = ClusterFileReader("path/to/file")
clusters = r.read()   # reads default amount of clusters
clusters = r.read(10) # read 10 clusters

```