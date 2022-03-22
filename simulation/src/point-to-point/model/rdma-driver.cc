#include "rdma-driver.h"

namespace ns3 {

/***********************
 * RdmaDriver
 **********************/
TypeId RdmaDriver::GetTypeId (void)
{
	static TypeId tid = TypeId ("ns3::RdmaDriver")
		.SetParent<Object> ()
		.AddTraceSource ("QpComplete", "A qp completes.",
				MakeTraceSourceAccessor (&RdmaDriver::m_traceQpComplete))
		;
	return tid;
}

RdmaDriver::RdmaDriver(){
}

void RdmaDriver::Init(void){
	Ptr<Ipv4> ipv4 = m_node->GetObject<Ipv4> ();
	#if 0
	m_rdma->m_nic.resize(ipv4->GetNInterfaces());
	for (uint32_t i = 0; i < m_rdma->m_nic.size(); i++){
		m_rdma->m_nic[i] = CreateObject<RdmaQueuePairGroup>();
		// share the queue pair group with NIC
		if (ipv4->GetNetDevice(i)->IsQbb()){
			DynamicCast<QbbNetDevice>(ipv4->GetNetDevice(i))->m_rdmaEQ->m_qpGrp = m_rdma->m_nic[i];
			//std:://cout<<"qbb net device is "<<m_rdma->m_nic[i]<<"\n";
		}
		//std:://cout<<"net device "<<i<<"\n"
	}
	#endif
	//std::cout<<"get n devices is "<<m_node->GetNDevices()<<"\n";
	for (uint32_t i = 0; i < m_node->GetNDevices(); i++){
		Ptr<QbbNetDevice> dev = NULL;
		if (m_node->GetDevice(i)->IsQbb())
			dev = DynamicCast<QbbNetDevice>(m_node->GetDevice(i));
		m_rdma->m_nic.push_back(RdmaInterfaceMgr(dev));
		m_rdma->m_nic.back().qpGrp = CreateObject<RdmaQueuePairGroup>();
		//std::cout<<"rdma driver nic size is "<<m_rdma->m_nic.size()<<"\n";
	}
	#if 0
	for (uint32_t i = 0; i < ipv4->GetNInterfaces (); i++){
		if (ipv4->GetNetDevice(i)->IsQbb() && ipv4->IsUp(i)){
			Ptr<QbbNetDevice> dev = DynamicCast<QbbNetDevice>(ipv4->GetNetDevice(i));
			// add a new RdmaInterfaceMgr for this device
			m_rdma->m_nic.push_back(RdmaInterfaceMgr(dev));
			m_rdma->m_nic.back().qpGrp = CreateObject<RdmaQueuePairGroup>();
			//std:://cout<<"rdma driver nic interface is "<<m_rdma->m_nic.size()<<"\n";
		}
	}
	#endif
	// RdmaHw do setup
	m_rdma->SetNode(m_node);
	m_rdma->Setup(MakeCallback(&RdmaDriver::QpComplete, this));
}

void RdmaDriver::SetNode(Ptr<Node> node){
	m_node = node;
}

void RdmaDriver::SetRdmaHw(Ptr<RdmaHw> rdma){
	m_rdma = rdma;
}

void RdmaDriver::AddQueuePair(uint32_t src, uint32_t dest, uint64_t tag, uint64_t size, uint16_t pg, Ipv4Address sip, Ipv4Address dip, uint16_t sport, uint16_t dport, uint32_t win, uint64_t baseRtt, Callback<void> notifyAppFinish, Callback<void> notifyAppSent){
	m_rdma->AddQueuePair(src, dest, tag, size, pg, sip, dip, sport, dport, win, baseRtt, notifyAppFinish, notifyAppSent);
}

void RdmaDriver::QpComplete(Ptr<RdmaQueuePair> q){
	m_traceQpComplete(q);
}

} // namespace ns3
