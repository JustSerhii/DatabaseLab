The domain you chose for the ER model should be selected as the domain. It is necessary to select two objects from the ER model, which are connected by a connection with the mapping type 1-∞. On the basis of the object to which 1 is directed, a master-file is formed, and on the basis of another - a slave-file. The program that works with these files must include a user interface (based on the command line or window), as well as the following functions:

1. the function of reading a given record and given sub-records based on direct index access; (get-m, get-s)// sub-records – records from the slave-file.
2. the function of extracting a given record or a given sub-record; when removing a record, it is necessary to remove all its sub-records; (del-m, del-s)
3. the function of updating the value of a given field of a given record or a given sub-record; (update-m, update-s)
4. the function of inserting a record and/or sub-records into files (insert-m, insert-s).
5. the function of counting the number of records, sub-records in general, as well as the number of sub-records for each record (calc-m, calc-s).
6. utilities (ut-m, ut-s) for reading and printing all fields (including service) master-file and slave-file.

So, in my case masters are users and slaves are reviews.
