# tCodebook
A text-based codebook tool on linux

![image](https://user-images.githubusercontent.com/35134755/167286193-c8cca7db-8975-494c-bd1c-7277c01f6c6a.png)

## Abstract
  tCodebook can helps those who work under CLI environment create their codebook and provide text-based management GUI.
In tCodebook, CRUD operation is allowed on your own codebook whenever you want. We also provide simple information statistics
to show which password is the most and least frequently used in all of your passwords. Moreover, the strongess will be
calculated on each of your password by our default rules, so you can clearly see how safe your password is and make a decision 
whether to change into another one.

## Usage:
Firstly, you should create a user to stands for yourself.
```
./tcb.out -c [new_user_name]
```

After creating, you can add an item into your codebook as follows:
```
./tcb.out -a [new_user_name]
```
then, enter the information of this item:

![image](https://user-images.githubusercontent.com/35134755/167287751-9eaa9a1f-60af-4d89-891f-1b183112c0bf.png)




![image](https://user-images.githubusercontent.com/35134755/167286376-6d6bed9d-ab3d-4f57-bc98-df0414a7b42b.png)

**Keywords:** C、ncurses、json-c

