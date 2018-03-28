from states import AEVCState
import time
from enum import Enum, auto

import teensy_talker as teensy


returnMessages = {}

def execute(rm):
    returnMessages.extend(rm)

class Initialize(AEVCState):

    def on_event(self, event):
        execute(teensy.home())
        return UnlockedState()


### Manual Control

class RC(Enum):
    forward = auto()
    turn = auto()
    turnBody = auto()
    turnBase = auto()
    height = auto()

class Manual(AEVCState):
    def on_event(self, event):

        command, value = event

        if command is RC.forward:
            teensy.moveForward(value)

        elif command is RC.turn:
            teensy.turn(value)

        elif command is RC.turnBody:
            teensy.turnBody(value)

        elif command is RC.turnBase:
            teensy.turnBase(value)

        elif command is RC.height:
            teensy.height(value)


### Automated Control

class Center(AEVCState):

    def on_event(self, event):
        if event == 'pin_entered':
            return Initialize()

        return self
