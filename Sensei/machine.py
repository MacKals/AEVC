import states as s

class AEVC(object):
    """
    A simple state machine that mimics the functionality of a device from a
    high level.
    """

    def __init__(self):
        self.state = Initialize()  # Start with initialization state

    def teensyEvent(self, event):

        if event in returnMessages:
            s.returnMessages.remove(event)
        else:
            print "error, tried to remove " + str(event) + " from " + str(returnMessages)

        if s.returnMessages.empty():
            print state + ": " + event
            self.state = self.state.on_event(event)
            s.returnMessages.extend(rm)



    def controllerEvent(self, event):
        # The next state will be the result of the on_event function.
