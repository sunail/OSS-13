base(_object, "Object", function()
	function New(this) end
	function IntreactedBy(this, o) return false end
	function InteractWith(this, o) end
end)

Projectile = function(t, v) return _projectile(t, v) end