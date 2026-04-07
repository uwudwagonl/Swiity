const char dashboard_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>Swiity Dashboard</title>
<style>
:root{--bg:#1a1a2e;--card:rgba(255,255,255,0.05);--txt:#e2e8f0;--dim:#94a3b8;--accent:#4f46e5;--green:#10b981;--red:#ef4444;--yellow:#fbbf24;--blue:#60a5fa;--pink:#f472b6;}
*{box-sizing:border-box;margin:0;padding:0;}
body{font-family:'Segoe UI',Tahoma,Geneva,Verdana,sans-serif;background:var(--bg);color:var(--txt);padding:1rem;display:flex;justify-content:center;}
.dash{width:100%;max-width:700px;}
h1{text-align:center;font-size:1.5rem;margin-bottom:1rem;}
.tabs{display:flex;gap:0.5rem;margin-bottom:1rem;}
.tab{flex:1;padding:0.6rem;text-align:center;background:var(--card);border:1px solid rgba(255,255,255,0.1);border-radius:8px;cursor:pointer;font-weight:600;font-size:0.85rem;color:var(--dim);transition:all 0.2s;}
.tab.active{background:var(--accent);color:#fff;border-color:var(--accent);}
.page{display:none;}.page.active{display:block;}
.card{background:var(--card);border-radius:12px;padding:1rem;margin-bottom:0.8rem;border:1px solid rgba(255,255,255,0.1);}
.section-title{font-size:0.8rem;text-transform:uppercase;letter-spacing:1px;color:var(--dim);margin-bottom:0.6rem;}
.slot{display:flex;justify-content:space-between;align-items:center;gap:0.8rem;margin-bottom:0.5rem;}.slot:last-child{margin-bottom:0;}
.slot-info{flex:1;}.slot-name{font-weight:600;font-size:1rem;}.slot-distance{color:var(--dim);font-size:0.85rem;}
.slot-status{font-size:0.85rem;font-weight:600;padding:0.3rem 0.6rem;border-radius:6px;min-width:70px;text-align:center;}
.present{background:rgba(16,185,129,0.2);color:var(--green);}
.empty{background:rgba(239,68,68,0.2);color:var(--red);}
.sensor-error{background:rgba(150,150,150,0.2);color:var(--dim);}
.thr-group{display:flex;align-items:center;gap:0.4rem;margin-top:0.3rem;}
.thr-group label{font-size:0.8rem;color:var(--dim);}
.thr-group input{width:55px;padding:0.2rem 0.3rem;border-radius:5px;border:1px solid rgba(255,255,255,0.2);background:rgba(0,0,0,0.3);color:var(--txt);font-size:0.8rem;text-align:center;}
.btn{padding:0.25rem 0.5rem;border-radius:5px;border:none;background:var(--accent);color:#fff;font-size:0.75rem;cursor:pointer;}.btn:hover{background:#4338ca;}
.btn-danger{background:var(--red);}.btn-danger:hover{background:#dc2626;}
.nfc-uid{font-size:1.5rem;font-weight:700;font-family:monospace;color:var(--yellow);margin:0.4rem 0;word-break:break-all;}
.nfc-uid.none{color:#475569;font-size:1rem;}
.nfc-meta{font-size:0.8rem;color:var(--dim);display:flex;justify-content:space-between;align-items:center;}
.stat-row{display:flex;gap:0.6rem;margin-bottom:0.8rem;flex-wrap:wrap;}
.stat-box{flex:1;min-width:100px;background:rgba(0,0,0,0.2);border-radius:10px;padding:0.7rem;text-align:center;}
.stat-val{font-size:1.4rem;font-weight:700;color:var(--blue);}.stat-label{font-size:0.7rem;color:var(--dim);margin-top:0.2rem;}
.filters{display:flex;gap:0.5rem;margin-bottom:0.8rem;flex-wrap:wrap;align-items:center;}
.filters select,.filters button{padding:0.35rem 0.6rem;border-radius:6px;border:1px solid rgba(255,255,255,0.2);background:rgba(0,0,0,0.3);color:var(--txt);font-size:0.8rem;}
.filters button{border:none;background:var(--accent);cursor:pointer;}
.filters label{font-size:0.8rem;color:var(--dim);display:flex;align-items:center;gap:0.3rem;}
.filters input[type=checkbox]{accent-color:var(--accent);}
.chart-container{position:relative;width:100%;overflow-x:auto;}
.chart{display:flex;align-items:flex-end;gap:2px;height:150px;padding:0.5rem 0;}
.chart-bar-group{display:flex;flex-direction:column;align-items:center;flex:1;min-width:8px;}
.chart-bar{width:100%;border-radius:3px 3px 0 0;min-height:1px;transition:height 0.3s;}
.chart-label{font-size:0.55rem;color:var(--dim);margin-top:0.2rem;white-space:nowrap;}
.chart-title{font-size:0.85rem;font-weight:600;margin-bottom:0.3rem;}
.chart-legend{display:flex;gap:1rem;margin-top:0.4rem;flex-wrap:wrap;}
.legend-item{display:flex;align-items:center;gap:0.3rem;font-size:0.75rem;color:var(--dim);}
.legend-dot{width:10px;height:10px;border-radius:2px;}
.occ-row{display:flex;align-items:center;gap:0.5rem;margin-bottom:0.4rem;}
.occ-label{width:50px;font-size:0.85rem;font-weight:600;}
.occ-bar-bg{flex:1;height:20px;background:rgba(0,0,0,0.3);border-radius:4px;overflow:hidden;}
.occ-bar{height:100%;border-radius:4px;transition:width 0.5s;}
.occ-val{width:40px;text-align:right;font-size:0.85rem;font-weight:600;}
.uptime-bar{font-size:0.85rem;color:var(--dim);text-align:center;margin-top:0.5rem;}
.no-data{text-align:center;color:var(--dim);padding:2rem;font-size:0.9rem;}
.tx-table{width:100%;border-collapse:collapse;font-size:0.8rem;}
.tx-table th{text-align:left;color:var(--dim);font-size:0.75rem;text-transform:uppercase;letter-spacing:0.5px;padding:0.4rem 0.5rem;border-bottom:1px solid rgba(255,255,255,0.1);}
.tx-table td{padding:0.4rem 0.5rem;border-bottom:1px solid rgba(255,255,255,0.05);}
.tx-table tr:hover td{background:rgba(255,255,255,0.03);}
.tx-uid{font-family:monospace;color:var(--yellow);font-weight:600;}
.tx-time{color:var(--dim);}
.tx-filters{display:flex;gap:0.5rem;align-items:center;margin-bottom:0.6rem;flex-wrap:wrap;}
.tx-filters input{padding:0.3rem 0.5rem;border-radius:5px;border:1px solid rgba(255,255,255,0.2);background:rgba(0,0,0,0.3);color:var(--txt);font-size:0.8rem;flex:1;min-width:120px;}
.tx-filters select{padding:0.3rem 0.5rem;border-radius:5px;border:1px solid rgba(255,255,255,0.2);background:rgba(0,0,0,0.3);color:var(--txt);font-size:0.8rem;}
.tx-count{font-size:0.75rem;color:var(--dim);margin-top:0.4rem;text-align:right;}
</style>
</head>
<body>
<div class="dash">
<h1>Swiity Dashboard</h1>
<div class="tabs">
  <div class="tab active" data-tab="live" onclick="switchTab('live')">Live</div>
  <div class="tab" data-tab="analytics" onclick="switchTab('analytics')">Analytics</div>
  <div class="tab" data-tab="transactions" onclick="switchTab('transactions')">Transactions</div>
</div>

<!-- LIVE PAGE -->
<div class="page active" id="page-live">
  <div class="card">
    <div class="section-title">NFC Reader</div>
    <div class="nfc-uid none" id="nfc-uid">Waiting...</div>
    <div class="nfc-meta">
      <span id="nfc-status">--</span>
      <button class="btn" onclick="fetch('/nfc/clear',{method:'POST'})">Clear</button>
    </div>
  </div>
  <div class="card">
    <div class="section-title">Product Slots</div>
    <div id="slots"></div>
  </div>
  <div class="card">
    <div class="uptime-bar">Uptime: <span id="uptime">--</span> | DB: <span id="db-info">--</span></div>
  </div>
</div>

<!-- ANALYTICS PAGE -->
<div class="page" id="page-analytics">
  <div class="stat-row" id="summary-stats">
    <div class="stat-box"><div class="stat-val" id="s-uptime">--</div><div class="stat-label">Uptime (h)</div></div>
    <div class="stat-box"><div class="stat-val" id="s-scans">--</div><div class="stat-label">Total Scans</div></div>
    <div class="stat-box"><div class="stat-val" id="s-uids">--</div><div class="stat-label">Unique Cards</div></div>
    <div class="stat-box"><div class="stat-val" id="s-records">--</div><div class="stat-label">Hours Logged</div></div>
    <div class="stat-box"><div class="stat-val" id="s-txcount">--</div><div class="stat-label">Transactions</div></div>
  </div>

  <div class="card">
    <div class="section-title">Filters</div>
    <div class="filters">
      <select id="f-range" onchange="loadAnalytics()">
        <option value="6">Last 6h</option>
        <option value="24" selected>Last 24h</option>
        <option value="72">Last 3 days</option>
        <option value="168">Last 7 days</option>
        <option value="0">All time</option>
      </select>
      <label><input type="checkbox" id="f-s1" checked onchange="loadAnalytics()"> Slot 1</label>
      <label><input type="checkbox" id="f-s2" checked onchange="loadAnalytics()"> Slot 2</label>
      <label><input type="checkbox" id="f-s3" checked onchange="loadAnalytics()"> Slot 3</label>
      <label><input type="checkbox" id="f-s4" checked onchange="loadAnalytics()"> Slot 4</label>
    </div>
  </div>

  <div class="card">
    <div class="chart-title">NFC Scans / Hour</div>
    <div class="chart-container"><div class="chart" id="chart-nfc"></div></div>
  </div>

  <div class="card">
    <div class="chart-title">Transactions Over Time</div>
    <div class="chart-container"><div class="chart" id="chart-tx"></div></div>
    <div style="font-size:0.7rem;color:var(--dim);margin-top:0.3rem;">Grouped by hour since boot</div>
  </div>

  <div class="card">
    <div class="chart-title">Slot Occupancy / Hour</div>
    <div class="chart-container"><div class="chart" id="chart-occ"></div></div>
    <div class="chart-legend" id="occ-legend"></div>
  </div>

  <div class="card">
    <div class="chart-title">Average Occupancy (selected range)</div>
    <div id="occ-summary"></div>
  </div>

  <div class="card" style="text-align:center;">
    <button class="btn btn-danger" onclick="if(confirm('Clear ALL analytics + transactions?'))fetch('/analytics/clear',{method:'POST'}).then(()=>{loadAnalytics();loadTransactions();})">Clear All Data</button>
  </div>
</div>

<!-- TRANSACTIONS PAGE -->
<div class="page" id="page-transactions">
  <div class="stat-row">
    <div class="stat-box"><div class="stat-val" id="tx-total">--</div><div class="stat-label">Total Transactions</div></div>
    <div class="stat-box"><div class="stat-val" id="tx-unique">--</div><div class="stat-label">Unique Cards</div></div>
    <div class="stat-box"><div class="stat-val" id="tx-rate">--</div><div class="stat-label">Scans / Hour</div></div>
  </div>

  <div class="card">
    <div class="chart-title">Transaction Activity</div>
    <div class="chart-container"><div class="chart" id="chart-tx-detail"></div></div>
    <div class="chart-legend">
      <div class="legend-item"><div class="legend-dot" style="background:var(--yellow)"></div>Transactions per hour</div>
    </div>
  </div>

  <div class="card">
    <div class="chart-title">Transactions by Card</div>
    <div class="chart-container"><div class="chart" id="chart-tx-cards" style="height:120px;"></div></div>
    <div class="chart-legend" id="card-legend"></div>
  </div>

  <div class="card">
    <div class="section-title">Transaction Log</div>
    <div class="tx-filters">
      <input type="text" id="tx-search" placeholder="Filter by UID..." oninput="renderTxTable()">
      <select id="tx-limit" onchange="renderTxTable()">
        <option value="20">Last 20</option>
        <option value="50" selected>Last 50</option>
        <option value="100">Last 100</option>
        <option value="0">All</option>
      </select>
    </div>
    <div style="max-height:400px;overflow-y:auto;">
      <table class="tx-table">
        <thead><tr><th>#</th><th>Time</th><th>Card UID</th></tr></thead>
        <tbody id="tx-body"></tbody>
      </table>
    </div>
    <div class="tx-count" id="tx-showing">--</div>
  </div>
</div>
</div>

<script>
const COLORS=['#60a5fa','#f472b6','#fbbf24','#34d399'];
const CARD_COLORS=['#fbbf24','#60a5fa','#f472b6','#34d399','#a78bfa','#fb923c','#e879f9','#2dd4bf'];
let txData=null;

function switchTab(t){
  document.querySelectorAll('.tab').forEach(el=>el.classList.toggle('active',el.dataset.tab===t));
  document.querySelectorAll('.page').forEach(el=>el.classList.remove('active'));
  document.getElementById('page-'+t).classList.add('active');
  if(t==='analytics')loadAnalytics();
  if(t==='transactions')loadTransactions();
}

function fmtTime(secs){
  const d=Math.floor(secs/86400),h=Math.floor((secs%86400)/3600),m=Math.floor((secs%3600)/60),s=secs%60;
  if(d>0)return d+'d '+h+'h '+m+'m';
  if(h>0)return h+'h '+m+'m';
  if(m>0)return m+'m '+s+'s';
  return s+'s';
}

function fmtShort(secs){
  const h=Math.floor(secs/3600),m=Math.floor((secs%3600)/60);
  return h+'h'+String(m).padStart(2,'0')+'m';
}

function setThreshold(slot,id){
  fetch('/threshold',{method:'POST',headers:{'Content-Type':'application/x-www-form-urlencoded'},body:'slot='+slot+'&value='+document.getElementById(id).value});
}

function updateLive(){
  fetch('/status').then(r=>r.json()).then(d=>{
    const u=document.getElementById('nfc-uid'),st=document.getElementById('nfc-status');
    if(!d.nfc.ready){u.textContent='Not connected';u.className='nfc-uid none';st.innerHTML='<span style="color:var(--red)">NFC offline</span>';}
    else if(!d.nfc.uid){u.textContent='No card scanned';u.className='nfc-uid none';st.textContent='Ready - tap a card';}
    else{u.textContent=d.nfc.uid;u.className='nfc-uid';st.textContent=d.nfc.age<2?'Just scanned':d.nfc.age+'s ago';}
    const c=document.getElementById('slots');
    if(!c.children.length){
      c.innerHTML=d.slots.map(s=>`<div class="slot"><div class="slot-info"><div class="slot-name">Slot ${s.slot}</div><div class="slot-distance">Distance: <span id="dist-${s.slot}">--</span> cm</div><div class="thr-group"><label>Thr (cm):</label><input type="number" id="thr-${s.slot}" value="${s.threshold}" step="0.5" min="1"><button class="btn" onclick="setThreshold(${s.slot},'thr-${s.slot}')">Set</button></div></div><div class="slot-status" id="status-${s.slot}">--</div></div>`).join('');
    }
    d.slots.forEach(s=>{
      const di=document.getElementById('dist-'+s.slot),st=document.getElementById('status-'+s.slot);
      if(s.distance<0){di.textContent='N/A';st.textContent='No sensor';st.className='slot-status sensor-error';}
      else{di.textContent=s.distance.toFixed(1);st.textContent=s.present?'Present':'Empty';st.className='slot-status '+(s.present?'present':'empty');}
    });
    document.getElementById('uptime').textContent=fmtTime(d.uptime);
    document.getElementById('db-info').textContent=d.dbRecords+'h logged | '+d.txCount+' transactions';
  }).catch(()=>{});
}

function loadAnalytics(){
  const range=document.getElementById('f-range').value;
  const url=range==='0'?'/analytics':'/analytics?last='+range;
  fetch(url).then(r=>r.json()).then(d=>{
    const slots=[document.getElementById('f-s1').checked,document.getElementById('f-s2').checked,document.getElementById('f-s3').checked,document.getElementById('f-s4').checked];
    const all=[...d.records,d.current];
    let totalScans=0,maxUids=0;
    all.forEach(r=>{totalScans+=r.nfc;if(r.uids>maxUids)maxUids=r.uids;});
    document.getElementById('s-uptime').textContent=d.uptime?Math.floor(d.uptime/3600):'0';
    document.getElementById('s-scans').textContent=totalScans;
    document.getElementById('s-uids').textContent=maxUids;
    document.getElementById('s-records').textContent=d.total;

    // Fetch tx count for summary
    fetch('/transactions?last=0').then(r=>r.json()).then(t=>{
      document.getElementById('s-txcount').textContent=t.total;
    }).catch(()=>{});

    // NFC chart
    const nfcChart=document.getElementById('chart-nfc');
    if(!all.length){nfcChart.innerHTML='<div class="no-data">No data yet</div>';}else{
      const mx=Math.max(1,...all.map(r=>r.nfc));
      nfcChart.innerHTML=all.map(r=>{
        const h=Math.max(1,Math.round((r.nfc/mx)*130));
        return `<div class="chart-bar-group"><div class="chart-bar" style="height:${h}px;background:var(--blue);" title="Hour ${r.h}: ${r.nfc} scans"></div><div class="chart-label">${r.h}</div></div>`;
      }).join('');
    }

    // Transaction timeline chart (from hourly NFC data)
    const txChart=document.getElementById('chart-tx');
    if(!all.length){txChart.innerHTML='<div class="no-data">No data yet</div>';}else{
      const mx=Math.max(1,...all.map(r=>r.nfc));
      txChart.innerHTML=all.map(r=>{
        const h=Math.max(1,Math.round((r.nfc/mx)*130));
        return `<div class="chart-bar-group"><div class="chart-bar" style="height:${h}px;background:var(--yellow);" title="Hour ${r.h}: ${r.nfc} transactions"></div><div class="chart-label">${r.h}</div></div>`;
      }).join('');
    }

    // Occupancy chart
    const occChart=document.getElementById('chart-occ');
    if(!all.length){occChart.innerHTML='<div class="no-data">No data yet</div>';}else{
      occChart.innerHTML=all.map(r=>{
        let bars='';
        for(let s=3;s>=0;s--){
          if(!slots[s])continue;
          const v=r.occ[s],h=Math.max(1,Math.round((v/100)*130));
          bars+=`<div class="chart-bar" style="height:${h}px;background:${COLORS[s]};opacity:0.8;" title="Slot ${s+1}: ${v}%"></div>`;
        }
        return `<div class="chart-bar-group" style="flex-direction:column-reverse;">${bars}<div class="chart-label">${r.h}</div></div>`;
      }).join('');
    }

    document.getElementById('occ-legend').innerHTML=[0,1,2,3].filter(s=>slots[s]).map(s=>`<div class="legend-item"><div class="legend-dot" style="background:${COLORS[s]}"></div>Slot ${s+1}</div>`).join('');

    let html='';
    for(let s=0;s<4;s++){
      if(!slots[s])continue;
      let sum=0;all.forEach(r=>sum+=r.occ[s]);
      const avg=all.length?Math.round(sum/all.length):0;
      html+=`<div class="occ-row"><div class="occ-label" style="color:${COLORS[s]}">Slot ${s+1}</div><div class="occ-bar-bg"><div class="occ-bar" style="width:${avg}%;background:${COLORS[s]};"></div></div><div class="occ-val">${avg}%</div></div>`;
    }
    document.getElementById('occ-summary').innerHTML=html||'<div class="no-data">Select at least one slot</div>';
  }).catch(()=>{});
}

function loadTransactions(){
  fetch('/transactions').then(r=>r.json()).then(d=>{
    txData=d;
    const txs=d.transactions;
    const total=d.total;
    const uptime=d.uptime;

    // Unique cards
    const uids=new Set(txs.map(t=>t.uid));
    document.getElementById('tx-total').textContent=total;
    document.getElementById('tx-unique').textContent=uids.size;
    const hours=uptime>0?uptime/3600:1;
    document.getElementById('tx-rate').textContent=(total/hours).toFixed(1);

    // Activity chart: group by hour
    const hourBuckets={};
    txs.forEach(tx=>{
      const h=Math.floor(tx.t/3600);
      hourBuckets[h]=(hourBuckets[h]||0)+1;
    });
    const hKeys=Object.keys(hourBuckets).map(Number).sort((a,b)=>a-b);
    const detailChart=document.getElementById('chart-tx-detail');
    if(!hKeys.length){detailChart.innerHTML='<div class="no-data">No transactions yet</div>';}else{
      const mx=Math.max(1,...hKeys.map(k=>hourBuckets[k]));
      detailChart.innerHTML=hKeys.map(h=>{
        const v=hourBuckets[h];
        const bh=Math.max(2,Math.round((v/mx)*130));
        return `<div class="chart-bar-group"><div class="chart-bar" style="height:${bh}px;background:var(--yellow);" title="Hour ${h}: ${v} tx"></div><div class="chart-label">${h}</div></div>`;
      }).join('');
    }

    // Per-card chart
    const cardCounts={};
    txs.forEach(tx=>{cardCounts[tx.uid]=(cardCounts[tx.uid]||0)+1;});
    const cardEntries=Object.entries(cardCounts).sort((a,b)=>b[1]-a[1]);
    const cardsChart=document.getElementById('chart-tx-cards');
    const cardLegend=document.getElementById('card-legend');
    if(!cardEntries.length){
      cardsChart.innerHTML='<div class="no-data">No transactions yet</div>';
      cardLegend.innerHTML='';
    }else{
      const mx=Math.max(1,...cardEntries.map(e=>e[1]));
      cardsChart.innerHTML=cardEntries.map((e,i)=>{
        const bh=Math.max(2,Math.round((e[1]/mx)*100));
        const color=CARD_COLORS[i%CARD_COLORS.length];
        return `<div class="chart-bar-group" style="flex:none;width:${Math.max(30,Math.floor(100/cardEntries.length))}px"><div class="chart-bar" style="height:${bh}px;background:${color};" title="${e[0]}: ${e[1]} tx"></div><div class="chart-label">${e[1]}</div></div>`;
      }).join('');
      cardLegend.innerHTML=cardEntries.map((e,i)=>`<div class="legend-item"><div class="legend-dot" style="background:${CARD_COLORS[i%CARD_COLORS.length]}"></div>${e[0].substring(0,11)}... (${e[1]})</div>`).join('');
    }

    renderTxTable();
  }).catch(()=>{});
}

function renderTxTable(){
  if(!txData)return;
  const search=document.getElementById('tx-search').value.toUpperCase();
  const limit=parseInt(document.getElementById('tx-limit').value);
  let txs=txData.transactions.slice().reverse();
  if(search)txs=txs.filter(t=>t.uid.includes(search));
  const total=txs.length;
  if(limit>0)txs=txs.slice(0,limit);
  const body=document.getElementById('tx-body');
  body.innerHTML=txs.map((tx,i)=>`<tr><td style="color:var(--dim)">${total-i}</td><td class="tx-time">${fmtShort(tx.t)} ago</td><td class="tx-uid">${tx.uid}</td></tr>`).join('');
  document.getElementById('tx-showing').textContent=`Showing ${txs.length} of ${total} transactions`;
}

setInterval(updateLive,800);
updateLive();
</script>
</body>
</html>
)rawliteral";
