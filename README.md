# tCodebook
A text-based codebook tool on linux

![image](https://user-images.githubusercontent.com/35134755/167286193-c8cca7db-8975-494c-bd1c-7277c01f6c6a.png)

## Abstract
tCodebook can help those who work under CLI environment create their codebook and provide text-based management GUI.
In tCodebook, CRUD operation is allowed on your own codebook whenever you want. We also provide simple information statistics
to show which password is the most and least frequently used in all of your passwords. Moreover, the strongness will be
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
Then, enter the information of this item:

<img src="https://user-images.githubusercontent.com/35134755/167287751-9eaa9a1f-60af-4d89-891f-1b183112c0bf.png" width="300">

Now you can use the text GUI mode to display all user's information or your codebook:
```
./tcb.out -g
```

<img src="https://user-images.githubusercontent.com/35134755/167289634-f36636cc-08e2-4695-891e-885470590b52.png" width="400">

Display all users:

<img src="https://user-images.githubusercontent.com/35134755/167289272-f147c2ec-e93b-45b3-8888-1d240af96bb6.png" width="700">

Display codebook:

<img src="https://user-images.githubusercontent.com/35134755/167289316-d09a5c58-d472-4ef5-ac84-abbdddc054d7.png" width="700">

Other functions can be executed as follows:

<img src="https://user-images.githubusercontent.com/35134755/167286376-6d6bed9d-ab3d-4f57-bc98-df0414a7b42b.png" width="800">

## Keywords:
C、ncurses、json-c

