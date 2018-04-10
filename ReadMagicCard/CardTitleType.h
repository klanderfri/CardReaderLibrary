#pragma once
enum CardTitleType {

	//The normal titel position
	NormalTitle = 1,

	//The card is split an as such has the titles on the side.
	SplitCardTitle = 2,
	
	//The card is split, with one title in the regular place.
	HouSplitCardTitle = 3,
	
	//The card has been transformed.
	TransformedTitle = 4,

	//The card has the design indicating a "future" card.
	FutureSightTitle = 5,

	//The card comes form the Amonkhet Invocations set and has the title in the regular place, written in semi-hieroglyphs.
	AmonkhetInvocationsTitle = 6,
};