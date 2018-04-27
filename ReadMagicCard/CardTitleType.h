#pragma once
//Enum specifying the kind of title a card has.
enum CardTitleType {

	//The normal titel position
	NormalTitle = 1,

	//The card is split an as such has the titles on the side.
	SplitCardTitle = 2,
	
	//The card is an Amonkhet split card, with one title in the regular place and one title in a split card half.
	AkhSplitCardTitle = 3,
	
	//The card has been transformed.
	TransformedTitle = 4,

	//The card has the design indicating a "future" card.
	FutureSightTitle = 5,

	//The card comes form the Amonkhet Invocations set and has the title in the regular place, written in semi-hieroglyphs.
	AmonkhetInvocationsTitle = 6,

	//The card is an emblem card.
	Emblem = 7,

	//The card is a token card.
	Token = 8,

	//The card is a normal transformed card, i.e the backside of the card.
	Backside = 9,

	//The card is a commercial card, only containing advertising.
	Commercial = 10
};