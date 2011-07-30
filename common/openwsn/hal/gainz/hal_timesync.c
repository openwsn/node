
static TiFunGetCurrentSystemTime m_getcurtime = NULL;
static void * m_systimer = NULL;
static bool m_resp_arrive = false;
static TiSystemTime m_sendtime;
static TiSystemTime m_backtime;

void hal_timesync_init( void * object, TiFunGetCurrentSystemTime getcurtime )
{
	m_systimer = object;
	m_getcurtime = getcurtime;
}

void hal_timesync_modify_request( TiFrame * frame )
{
	m_getcurtime( m_systimer, &m_sendtime );
	
	// assert( the current frame is a TimeSync REQUEST frame 
	// todo: load the m_sendtime into the frame
	
	m_resp_arrive = false;
}

void hal_timesync_on_respones_arrival( TiFrame * frame )
{
	m_getcurtime( m_systimer, &m_sendtime );
	m_resp_arrive = true;
}

bool hal_timesync_retrieve( TiSystemTime * sendtime, TiSystemTime * backtime )
{
	if (m_resp_arrive)
	{
		*sendtime = m_sendtime;
		*backtime = m_backtime;
		return true;
	}
	else
		return false;
}
