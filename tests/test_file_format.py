"""Testing error cases related to file formats.

- Each file contains the same, valid geometry. Thus the various formatting 
options and errors are tested. 

- Invalid schema or geometry definitions, eg invalid CityGML
"""

## 901

import pytest
import os.path

#------------------------------------------------------------------------ Data
@pytest.fixture(scope="module",
                params=["invalid_citygml_1.gml",
                        "invalid_citygml_2.gml"])
def data_invalid_citygml(request, dir_file_format):
    file_path = os.path.abspath(
        os.path.join(
            dir_file_format,
            request.param))
    return([file_path])

@pytest.fixture(scope="module",
                params=["composite_solid.gml",
                        "composite_solid_1.gml"])
def data_namespace(request, dir_valid):
    file_path = os.path.abspath(
        os.path.join(
            dir_valid,
            request.param))
    return([file_path])

#----------------------------------------------------------------------- Tests
@pytest.mark.full
def test_invalid_citygml(validate, data_invalid_citygml, citymodel):
    error = validate(data_invalid_citygml, options=citymodel)
    assert(error == [901])

@pytest.mark.full
def test_namespace(val3dity, validate_full, data_namespace):
    message = "CityGML input file"
    command = [val3dity] + data_namespace
    out, err = validate_full(command)
    assert message in out
    