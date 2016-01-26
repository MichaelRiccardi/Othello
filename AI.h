class AI{
	private:
		int playerType;
		int bestX;				// AI move x - coordinate
	    int bestY;				// AI move y - coordinate
	    int maxPoints;		// Best move is the one that gets the most points

	public:
		AI(); // where N is the array size for the possible moves

		void setType(int Color);

		void makeMoveAI(); // Determines the best move for the AI to make
		void maxValue(int points, int x, int y);
};