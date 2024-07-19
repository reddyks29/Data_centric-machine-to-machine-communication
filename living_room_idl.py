import rti.types as idl

@idl.struct
class Temp:
    temp: float=0.0

@idl.struct
class Humi:
    humi: float=0.0

@idl.struct
class Pressure:
    pressure: float=0.0

@idl.struct
class Servo:
    servo:float=0.0

@idl.struct
class DCmotor:
    dcmotor:float=0.0