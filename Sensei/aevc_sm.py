from aevc_states import Initialize


class AEVC(object):
    """
    A simple state machine that mimics the functionality of a device from a
    high level.
    """

    def __init__(self):
        """ Initialize the components. """

        # Start with a initialization
        self.state = Initialize()

    def on_event(self, event):

        # The next state will be the result of the on_event function.
        self.state = self.state.on_event(event)


