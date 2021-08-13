default:
	cd OBJ; make
install:
	cd OBJ; make install
clean:
	cd OBJ; make clean
distclean:
	rm -fr OBJ copy.out Makefile
