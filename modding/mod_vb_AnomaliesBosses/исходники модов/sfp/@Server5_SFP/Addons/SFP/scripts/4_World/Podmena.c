
class SFP_ExoClassic_Tors_Dolg extends Clothing {
	override void EEKilled(Object killer)
	{
		string newWheel = "";
		switch( GetType() )
		{
			case "SFP_ExoClassic_Tors_Dolg":
				newWheel = "SFP_ExoClassic_TorsRuined_Dolg";
			break;
		}
		if ( newWheel != "" )
        {
            ReplaceWheelLambda lambda = new ReplaceWheelLambda ( this, newWheel, NULL);
            lambda.SetTransferParams(true, true, true);
            GetInventory().ReplaceItemWithNew(InventoryMode.SERVER, lambda);
        }
        super.EEKilled( killer );
    }
};

class SFP_ExoClassic_Tors_Svoboda extends Clothing {
	override void EEKilled(Object killer)
	{
		string newWheel = "";
		switch( GetType() )
		{
			case "SFP_ExoClassic_Tors_Svoboda":
				newWheel = "SFP_ExoClassic_TorsRuined_Svoboda";
			break;
		}
		if ( newWheel != "" )
        {
            ReplaceWheelLambda lambda = new ReplaceWheelLambda ( this, newWheel, NULL);
            lambda.SetTransferParams(true, true, true);
            GetInventory().ReplaceItemWithNew(InventoryMode.SERVER, lambda);
        }
        super.EEKilled( killer );
    }
};

class SFP_ExoClassic_Tors_Naem extends Clothing {
	override void EEKilled(Object killer)
	{
		string newWheel = "";
		switch( GetType() )
		{
			case "SFP_ExoClassic_Tors_Naem":
				newWheel = "SFP_ExoClassic_TorsRuined_Naem";
			break;
		}
		if ( newWheel != "" )
        {
            ReplaceWheelLambda lambda = new ReplaceWheelLambda ( this, newWheel, NULL);
            lambda.SetTransferParams(true, true, true);
            GetInventory().ReplaceItemWithNew(InventoryMode.SERVER, lambda);
        }
        super.EEKilled( killer );
    }
};

class SFP_ExoClassic_Tors_monolit extends Clothing {
	override void EEKilled(Object killer)
	{
		string newWheel = "";
		switch( GetType() )
		{
			case "SFP_ExoClassic_Tors_monolit":
				newWheel = "SFP_ExoClassic_TorsRuined_monolit";
			break;
		}
		if ( newWheel != "" )
        {
            ReplaceWheelLambda lambda = new ReplaceWheelLambda ( this, newWheel, NULL);
            lambda.SetTransferParams(true, true, true);
            GetInventory().ReplaceItemWithNew(InventoryMode.SERVER, lambda);
        }
        super.EEKilled( killer );
    }
};

class SFP_ExoClassic_Tors_Neitral extends Clothing {
	override void EEKilled(Object killer)
	{
		string newWheel = "";
		switch( GetType() )
		{
			case "SFP_ExoClassic_Tors_Neitral":
				newWheel = "SFP_ExoClassic_TorsRuined_Neitral";
			break;
		}
		if ( newWheel != "" )
        {
            ReplaceWheelLambda lambda = new ReplaceWheelLambda ( this, newWheel, NULL);
            lambda.SetTransferParams(true, true, true);
            GetInventory().ReplaceItemWithNew(InventoryMode.SERVER, lambda);
        }
        super.EEKilled( killer );
    }
};

class SFP_ExoClassic_Pants_Dolg extends Clothing {
	override void EEKilled(Object killer)
	{
		string newWheel = "";
		switch( GetType() )
		{
			case "SFP_ExoClassic_Pants_Dolg":
				newWheel = "SFP_ExoClassic_PantsRuined_Dolg";
			break;
		}
		if ( newWheel != "" )
        {
            ReplaceWheelLambda lambda = new ReplaceWheelLambda ( this, newWheel, NULL);
            lambda.SetTransferParams(true, true, true);
            GetInventory().ReplaceItemWithNew(InventoryMode.SERVER, lambda);
        }
        super.EEKilled( killer );
    }
};
class SFP_ExoClassic_Pants_Svoboda extends Clothing {
	override void EEKilled(Object killer)
	{
		string newWheel = "";
		switch( GetType() )
		{
			case "SFP_ExoClassic_Pants_Svoboda":
				newWheel = "SFP_ExoClassic_PantsRuined_Svoboda";
			break;
		}
		if ( newWheel != "" )
        {
            ReplaceWheelLambda lambda = new ReplaceWheelLambda ( this, newWheel, NULL);
            lambda.SetTransferParams(true, true, true);
            GetInventory().ReplaceItemWithNew(InventoryMode.SERVER, lambda);
        }
        super.EEKilled( killer );
    }
};
class SFP_ExoClassic_Pants_Naem extends Clothing {
	override void EEKilled(Object killer)
	{
		string newWheel = "";
		switch( GetType() )
		{
			case "SFP_ExoClassic_Pants_Naem":
				newWheel = "SFP_ExoClassic_PantsRuined_Naem";
			break;
		}
		if ( newWheel != "" )
        {
            ReplaceWheelLambda lambda = new ReplaceWheelLambda ( this, newWheel, NULL);
            lambda.SetTransferParams(true, true, true);
            GetInventory().ReplaceItemWithNew(InventoryMode.SERVER, lambda);
        }
        super.EEKilled( killer );
    }
};
class SFP_ExoClassic_Pants_monolit extends Clothing {
	override void EEKilled(Object killer)
	{
		string newWheel = "";
		switch( GetType() )
		{
			case "SFP_ExoClassic_Pants_monolit":
				newWheel = "SFP_ExoClassic_PantsRuined_monolit";
			break;
		}
		if ( newWheel != "" )
        {
            ReplaceWheelLambda lambda = new ReplaceWheelLambda ( this, newWheel, NULL);
            lambda.SetTransferParams(true, true, true);
            GetInventory().ReplaceItemWithNew(InventoryMode.SERVER, lambda);
        }
        super.EEKilled( killer );
    }
};
class SFP_ExoClassic_Pants_Neitral extends Clothing {
	override void EEKilled(Object killer)
	{
		string newWheel = "";
		switch( GetType() )
		{
			case "SFP_ExoClassic_Pants_Neitral":
				newWheel = "SFP_ExoClassic_PantsRuined_Neitral";
			break;
		}
		if ( newWheel != "" )
        {
            ReplaceWheelLambda lambda = new ReplaceWheelLambda ( this, newWheel, NULL);
            lambda.SetTransferParams(true, true, true);
            GetInventory().ReplaceItemWithNew(InventoryMode.SERVER, lambda);
        }
        super.EEKilled( killer );
    }
};
class SFP_ExoClassic_Helmet_Dolg extends Clothing {
	override void EEKilled(Object killer)
	{
		string newWheel = "";
		switch( GetType() )
		{
			case "SFP_ExoClassic_Helmet_Dolg":
				newWheel = "SFP_ExoClassic_HelmetRuined_Dolg";
			break;
		}
		if ( newWheel != "" )
        {
            ReplaceWheelLambda lambda = new ReplaceWheelLambda ( this, newWheel, NULL);
            lambda.SetTransferParams(true, true, true);
            GetInventory().ReplaceItemWithNew(InventoryMode.SERVER, lambda);
        }
        super.EEKilled( killer );
    }
};
class SFP_ExoClassic_Helmet_Svoboda extends Clothing {
	override void EEKilled(Object killer)
	{
		string newWheel = "";
		switch( GetType() )
		{
			case "SFP_ExoClassic_Helmet_Svoboda":
				newWheel = "SFP_ExoClassic_HelmetRuined_Svoboda";
			break;
		}
		if ( newWheel != "" )
        {
            ReplaceWheelLambda lambda = new ReplaceWheelLambda ( this, newWheel, NULL);
            lambda.SetTransferParams(true, true, true);
            GetInventory().ReplaceItemWithNew(InventoryMode.SERVER, lambda);
        }
        super.EEKilled( killer );
    }
};
class SFP_ExoClassic_Helmet_Naem extends Clothing {
	override void EEKilled(Object killer)
	{
		string newWheel = "";
		switch( GetType() )
		{
			case "SFP_ExoClassic_Helmet_Naem":
				newWheel = "SFP_ExoClassic_HelmetRuined_Naem";
			break;
		}
		if ( newWheel != "" )
        {
            ReplaceWheelLambda lambda = new ReplaceWheelLambda ( this, newWheel, NULL);
            lambda.SetTransferParams(true, true, true);
            GetInventory().ReplaceItemWithNew(InventoryMode.SERVER, lambda);
        }
        super.EEKilled( killer );
    }
};
class SFP_ExoClassic_Helmet_monolit extends Clothing {
	override void EEKilled(Object killer)
	{
		string newWheel = "";
		switch( GetType() )
		{
			case "SFP_ExoClassic_Helmet_monolit":
				newWheel = "SFP_ExoClassic_HelmetRuined_monolit";
			break;
		}
		if ( newWheel != "" )
        {
            ReplaceWheelLambda lambda = new ReplaceWheelLambda ( this, newWheel, NULL);
            lambda.SetTransferParams(true, true, true);
            GetInventory().ReplaceItemWithNew(InventoryMode.SERVER, lambda);
        }
        super.EEKilled( killer );
    }
};
class SFP_ExoClassic_Helmet_Neitral extends Clothing {
	override void EEKilled(Object killer)
	{
		string newWheel = "";
		switch( GetType() )
		{
			case "SFP_ExoClassic_Helmet_Neitral":
				newWheel = "SFP_ExoClassic_HelmetRuined_Neitral";
			break;
		}
		if ( newWheel != "" )
        {
            ReplaceWheelLambda lambda = new ReplaceWheelLambda ( this, newWheel, NULL);
            lambda.SetTransferParams(true, true, true);
            GetInventory().ReplaceItemWithNew(InventoryMode.SERVER, lambda);
        }
        super.EEKilled( killer );
    }
};

class SFP_ExoClassic_HelmetRuined_Neitral extends Clothing {};
class SFP_ExoClassic_HelmetRuined_monolit extends Clothing {};
class SFP_ExoClassic_HelmetRuined_Naem extends Clothing {};
class SFP_ExoClassic_HelmetRuined_Svoboda extends Clothing {};
class SFP_ExoClassic_HelmetRuined_Dolg extends Clothing {};
class SFP_ExoClassic_PantsRuined_monolit extends Clothing {};
class SFP_ExoClassic_PantsRuined_Naem extends Clothing {};
class SFP_ExoClassic_PantsRuined_Svoboda extends Clothing {};
class SFP_ExoClassic_PantsRuined_Dolg extends Clothing {};
class SFP_ExoClassic_TorsRuined_Neitral extends Clothing {};
class SFP_ExoClassic_TorsRuined_Naem extends Clothing {};
class SFP_ExoClassic_TorsRuined_monolit extends Clothing {};
class SFP_ExoClassic_TorsRuined_Svoboda extends Clothing {};
class SFP_ExoClassic_TorsRuined_Dolg extends Clothing {};
