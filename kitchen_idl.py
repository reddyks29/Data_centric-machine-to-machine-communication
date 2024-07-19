import rti.types as idl

@idl.struct
class Temp:
    temp: float=0.0

@idl.struct
class Humi:
    humi: float=0.0

@idl.struct
class Smoke:
    smokelvl: float=0.0

@idl.struct
class IR:
    ir_data:float=0.0