#This is a single interface to various makefile arguments within each folder.
#Just for convenience.
SENDER=sion_sender
RECEIVER=ciel_receiver
CAN=sion_can

.PHONY: all $(CAN) $(CAN)_program $(SENDER) $(SENDER)_deploy $(RECEIVER) clean


all : $(CAN) $(SENDER) $(RECEIVER)

$(CAN) : 
	(cd $(CAN); make all)

$(CAN)_program :
	(cd $(CAN); make program)

$(SENDER) :
	(cd $(SENDER); make $(SENDER))

$(SENDER)_deploy :
	(cd $(SENDER); make deploy)

$(RECEIVER) :
	(cd $(RECEIVER); make $(RECEIVER))

clean :
	(cd $(CAN); make clean)
	(cd $(SENDER); make clean)
	(cd $(RECEIVER); make clean)
