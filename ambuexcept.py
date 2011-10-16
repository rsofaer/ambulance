class AmbulanceException(Exception):
    pass

class FormatException(AmbulanceException):
    def __init__(self, ambunum, action):
        self.ambunum, self.action = ambunum, action
    def msg(self):
        return 'Unknow action {} for ambulance {}.' \
               .format(self.action, self.ambunum)

class StartHospitalException(AmbulanceException):
    def __init__(self, ambunum):
        self.ambunum = ambunum
    def msg(self):
        return 'You have to start at a hospital for ambulance {}.' \
               .format(self.ambunum)

class UnloadHospitalException(AmbulanceException):
    def __init__(self, ambunum):
        self.ambunum = ambunum
    def msg(self):
        return 'You have to unload all your patients on ambulace {} ' + \
               'to a hospital.'.format(self.ambunum)

class PeopleDieException(AmbulanceException):
    def __init__(self, ambunum, patient, patinfo, hospital, time):
        self.ambunum, self.patient, self.patinfo, self.hospital, self.time = \
                      ambunum, patient, patinfo, hospital, time
    def msg(self):
        return ('Ambulance {} tries to save patient {}{}, ' +
                'but unloads at hospital {} at time {}.') \
                .format(self.ambunum, self.patient, self.patinfo, \
                        self.hospital, self.time)

class SaveTwiceException(AmbulanceException):
    def __init__(self, ambunum, patient, hostpital, savedtuple, time):
        self.ambunum, self.patient, self.hostpital, self.savedtuple, \
                      self.time = \
                      ambunum, patient, hostpital, savedtuple, time
    def msg(self):
        sa, sh, st = savedtuple
        return ('Patient {} was saved by ambulance {} at hospital {} ' + \
                'at time {}, and is now sent to hospital {} ' + \
                'at time {} by ambulance {}.') \
                .format(self.patient, sa, sh, st, \
                        self.hospital, self.time, self.ambunum)

class OverloadException(AmbulanceException):
    def __init__(self, ambunum, patient, time):
        self.ambunum, self.patient, self.time = ambunum, patient, time
    def msg(self):
        return ('Ambulance {} is overloaded when adding patient {} at ' + \
                'time {}.').format(self.ambunum, self.patient, self.time)

class MoreAmbulanceException(AmbulanceException):
    def __init__(self, ambunum, hospital):
        self.ambunum, self.hospital = ambunum, hospital
    def msg(self):
        return ('More ambulances (line number {}) from hospital {} ' + \
                'than allowed.').format(self.ambunum, self.hospital)
