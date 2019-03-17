
class Order():
    def __init__(self):
        self.game_class = None
        self.rating_from = None
        self.rating_to = None
        self.fraction = None
        self.server = None
        self.game_mode = None

    def setGameClass(self, gameClass):
        self.game_class = gameClass

    def setRatingFrom(self, ratingFrom):
        self.rating_from = ratingFrom

    def setRatingTo(self, ratingTo):
        self.rating_to = ratingTo

    def setFraction(self, fraction):
        self.fraction = fraction

    def setServer(self, server):
        self.server = server

    def setGameMode(self, mode):
        self.game_mode = mode

    def getGameClass(self):
        return self.game_class

    def getRatingFrom(self):
        return self.rating_from

    def getRatingTo(self):
        return self.rating_to

    def getFraction(self):
        return self.fraction

    def getGameServer(self):
        return self.server

    def getGameMode(self):
        return self.game_mode