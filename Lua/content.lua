class(Object, "Turf", function()
	function New(this)
		Object.New(this);
		this.layer = 25;
		this.movable = false;
	end
end)


class(Object, "Uniform", function()
	function New(this)
		Object.New(this);
		this.sprite = "grey";
		this.slot = 2;
	end
end)


class(Turf, "Wall", function()
	function New(this)
		Turf.New(this);
		this.sprite = "wall";
		this.name = "Wall";
		this.density = true;
	end
end)


class(Turf, "Floor", function()
	function New(this)
		Turf.New(this);
		this.layer = 15;
		this.sprite = "floor";
		this.name = "Floor";
		this.density = false;
	end
end)


class(Object, "Taser", function()
    function New(this)
        Object.New(this);
        this.sprite = "taser";
    end
    function InteractWith(this, o)
        if not o or not o:GetTile() or not this:GetTile() then return; end;
        Projectile(this:GetTile(), o:GetTile():GetPos() - this:GetTile():GetPos());
    end
end)