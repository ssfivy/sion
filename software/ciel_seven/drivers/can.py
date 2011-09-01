# SION-like CAN packet container


class sion_entry():
	def __init__(self, priority, message_type, source_address, specifics, value, scandal_timestamp=None, ciel_timestamp=None, pkt_id=None):
		self.priority=priority
		self.message_type=message_type
		self.source_address=source_address
		self.specifics=specifics
		self.value=value
		self.scandal_timestamp=scandal_timestamp
		self.ciel_timestamp=ciel_timestamp
	def string(self):
		string=""
		string += chr(self.priority)
		string += chr(self.message_type)
		string += chr(self.source_address)
		string += '\x00' #HACK
		string += chr(self.specifics)

		string += chr(self.value>>24 & '\xFF')
		string += chr(self.value>>16  & '\xFF')
		string += chr(self.value>>8 & '\xFF')
		string += chr(self.value & '\xFF')

		string += chr(self.value >> 56 & '\xFF')
		string += chr(self.value >> 48 & '\xFF')
		string += chr(self.value >> 40 & '\xFF')
		string += chr(self.value >> 32 & '\xFF')
		string += chr(self.value >> 24 & '\xFF')
		string += chr(self.value >> 16 & '\xFF')
		string += chr(self.value >> 8 & '\xFF')
		string += chr(self.value >> 0 & '\xFF')

		string[17:20] += '\x00\x00\x00\x00'
		self.longstring = string 

