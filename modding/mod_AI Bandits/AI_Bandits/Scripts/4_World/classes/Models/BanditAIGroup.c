class BanditAIGroup
{
	protected string m_Faction;
	private autoptr BanditAIFormation m_Formation = new BanditAIFormationVee();
	protected InfectedBanditBase m_Leader;
	private autoptr array<InfectedBanditBase> m_Members;
	private autoptr array<EntityAI> m_Targets;
	protected TStringArray m_Waypoints;
	protected int m_WaypointIndex;
	
	void BanditAIGroup()
	{
		m_Members = new array<InfectedBanditBase>;
		m_Targets = new array<EntityAI>;
	}
	
	void SetName(string name)
	{
	}
	
	void SetFaction(string faction)
	{
		m_Faction = faction;
	}
	
	string GetFaction()
	{
		return m_Faction;
	}
		
	void AddTarget(EntityAI target)
	{
		m_Targets.Insert(target);
	}
	
	void DeleteTarget(EntityAI target)
	{
		m_Targets.Remove( m_Targets.Find(target) );
	}
	
	bool IsTarget(EntityAI target)
	{
		int index = m_Targets.Find(target);
		if (index == -1)
		{
			return false;
		}
		return true;
	}
	
	void SetWaypoints(TStringArray waypoints)
	{
		m_Waypoints = waypoints;
	}
	
	TStringArray GetWayPoints()
	{
		return m_Waypoints;
	}
	
	void SetWaypointIndex(int index)
	{
		m_WaypointIndex = index;
	}
	
	int GetWaypointIndex()
	{
		return m_WaypointIndex;
	}
	
	void SetLeader(InfectedBanditBase leader)
	{
		m_Leader = leader;
		AddMember(m_Leader);
		//m_Leader.SetWaypoints(m_Waypoints);
	}
	
	InfectedBanditBase GetLeader()
	{
		return m_Leader;
	}
	
	void AddMember(InfectedBanditBase member)
	{
		m_Members.Insert(member);
		int index = m_Members.Find(member);
		//Print("BanditAI index " + index);
		member.SetWaypointIndex(m_WaypointIndex);
		member.SetWaypoints(m_Waypoints);
	}
	
	int GetMemberID(InfectedBanditBase member)
	{
		int index = m_Members.Find(member);
		return index;
	}
	
	void RemoveMember(InfectedBanditBase member)
	{
		int index = m_Members.Find(member);
		m_Members.Remove(index);
	}
	
	void SetFormation(BanditAIFormation formation)
	{
		m_Formation = formation;
	}
	
	BanditAIFormation GetFormation()
	{
		return m_Formation;
	}
	
};