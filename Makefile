default: model

CXXFLAGS=-fpermissive -g -O2

SIMLIB=simlib
BASEOBJFILES = atexit.o \
	calendar.o debug.o \
	entity.o error.o errors.o event.o \
	link.o list.o name.o \
	object.o \
	print.o run.o \
	sampler.o \
	opt-hooke.o opt-simann.o opt-param.o

CONTIOBJFILES = delay.o zdelay.o simlib2D.o simlib3D.o\
	algloop.o cond.o \
	fun.o graph.o \
	intg.o continuous.o ni_abm4.o ni_euler.o \
	ni_fw.o ni_rke.o ni_rkf3.o ni_rkf5.o ni_rkf8.o numint.o \
	output1.o \
	stdblock.o

DISCOBJFILES = \
	barrier.o \
	facility.o \
	histo.o \
	output2.o process.o queue.o random1.o random2.o \
	semaphor.o stat.o store.o tstat.o waitunti.o

OBJFILES = $(BASEOBJFILES) $(CONTIOBJFILES) $(DISCOBJFILES)

simlib.so: ${SIMLIB}/version.o $(addprefix $(SIMLIB)/,$(OBJFILES))
	$(CXX) -fpermissive -shared -o $@ $^

model: model.cc simlib.so
#	$(CXX) -g -O2 -o $@ $^ -lsimlib -lm
	$(CXX) -g -O2 -I${SIMLIB} -o $@ $^ -lm
run: model
	LD_LIBRARY_PATH=.:$(LD_LIBRARY_PATH) ./model
