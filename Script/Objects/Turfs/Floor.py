from Objects.Turf import Turf

class Floor(Turf):
	def __init__(self):
		super().__init__()
		self.layer = 15
		self.sprite = "floor"
		self.name = "Floor"
