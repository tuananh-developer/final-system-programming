# Test Cases

| ID    | Scenario                     | Expected Result                                      |
| ----- | ---------------------------- | ---------------------------------------------------- |
| TC-01 | Copy a text file             | Destination content matches source                   |
| TC-02 | Copy a binary file           | Destination bytes match source                       |
| TC-03 | Copy an empty file           | Empty destination file is created                    |
| TC-04 | Copy a nested directory      | All supported files and subdirectories are copied    |
| TC-05 | Copy an empty directory      | Empty destination directory is created               |
| TC-06 | Source does not exist        | Program reports an error                             |
| TC-07 | Destination already exists   | Program reports an error; existing data is unchanged |
| TC-08 | Destination is inside source | Program rejects the operation                        |
| TC-09 | Source is a symbolic link    | Program rejects the operation                        |
| TC-10 | Missing read permission      | Copy fails and an error is reported                  |
