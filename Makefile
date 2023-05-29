DEFAULT_ARGS = 2 3

all: parent helper spy sender receiver master_spy child

parent: parent.c
	gcc -g parent.c -o parent

spy: spy.c
	gcc -g spy.c -o spy

helper: helper.c
	gcc -g spy.c -o helper

sender: sender.c
	gcc -g sender.c -o sender
	
receiver: receiver.c
	gcc -g receiver.c -o receiver

masterSpy: masterSpy.c
	gcc -g master_spy.c -o master_spy

child: child.c
	gcc -g child.c -o child

run:
	./parent $(ARGS)

default-run:
	./parent $(DEFAULT_ARGS)

clean:
	rm -f parent spy receiver master_spy child sender helper 