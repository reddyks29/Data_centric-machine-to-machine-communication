import rti.types as idl

@idl.struct
class Survey:
    survey: float=0.0

@idl.struct
class Parking:
    parking: float=0.0

@idl.struct
class Water:
    water: float=0.0
