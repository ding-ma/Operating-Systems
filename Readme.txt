To compile, simply run:
1- cmake .
2- make
This will generate a3_test executable. There will create libsma.a for the library file where it can be attached to other program.

Some issues that the code has:
* Test 5 does not pass, I believe the code there is correct but it is not pointing to the right address.
However, the location of the malloc still contains "310" and "427".
There is a put in the tester to demonstrate that.

* In order to get Test 3 and Test 4 to pass, I need to add a sma_free at the end of every run for test 2.
Otherwise it will create too many blocks and Test 3,4 will fail.
Another alternative is to run Test 2 after Test 5 where no free is needed. That code is in the tester.
The reason behind this is because during Test 3, when we free the blocks, they are not the biggest blocks in the memory list.
Hence, the worse fit algorithm will not find those addresses.