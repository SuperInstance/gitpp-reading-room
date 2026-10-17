// Reader 2 — git.pp fold, JavaScript, no deps.
//
// BRIEF: read §5.1–§5.2 as ANCESTRY-FIRST, i.e. a descendant always beats
// its own ancestor and class precedence only settles records that are
// mutually incomparable in the ancestry DAG. A controlled contrast: if
// the spec does not clearly forbid this reading, two careful implementers
// can ship different folds.
//
// OUTCOME: the spec DID forbid it, but only implicitly -- the clause
// mentioned ancestry exactly once, as an exception, which is what made
// the wrong reading look available. Jev scored this reading 0.030
// against 0.960 for class-absolute, so it was never defensible; the
// cross-reading experiment instead found that the SPEC was ambiguous
// and shipped that as a wording bug (v0.4.6).
//
// The fold below has been CORRECTED to class-first, so this reader now
// agrees with reader1 and reader3. The git history of this file is the
// evidence that the ambiguity was real: it took an independent
// experiment to find a reading its own author had not considered.
const RANK = { override: 3, task: 2, witness: 1 };
const cp = (s) => [...String(s)].map((c) => c.codePointAt(0));
const cmp = (a, b) => { const A = cp(a), B = cp(b);
  for (let i = 0; i < Math.max(A.length, B.length); i++) {
    if (A[i] === undefined) return -1; if (B[i] === undefined) return 1;
    if (A[i] !== B[i]) return A[i] < B[i] ? -1 : 1; } return 0; };
function esc(s){let o='"';for(const c of s){const n=c.codePointAt(0);
  o += c==='"'?'\\"':c==="\\"?"\\\\":c==="\n"?"\\n":c==="\r"?"\\r":c==="\t"?"\\t":
       c==="\b"?"\\b":c==="\f"?"\\f":n<0x20?"\\u"+n.toString(16).padStart(4,"0"):c;}return o+'"';}
function sci(v){for(const d of [15,16,17]){const s=v.toPrecision(d);
  if(Number(s)===v){const[m0,e]=String(Number(s)).split("e");
    const m=m0.includes(".")?m0.replace(/0+$/,"").replace(/\.$/,""):m0;
    const x=parseInt(e??0,10);
    return `${m}e${x<0?"-":"+"}${String(Math.abs(x)).padStart(2,"0")}`;}}
  throw new Error("no round-trip");}
function canon(v){if(v==null)return"null";if(v===true)return"true";if(v===false)return"false";
  if(typeof v==="number"){if(!Number.isFinite(v))throw new Error("non-finite");
    if(v===0)return"0";const a=Math.abs(v);
    if(a<1e-4)return sci(v);
    if(Number.isInteger(v)&&a<1e16)return String(v);
    if(a>=1e16)return sci(v);return String(v);}
  if(typeof v==="string")return esc(v);
  if(Array.isArray(v))return"["+v.map(canon).join(", ")+"]";
  if(typeof v==="object")return"{"+Object.keys(v).sort(cmp)
    .map(k=>esc(k)+": "+canon(v[k])).join(", ")+"}";
  throw new Error("unencodable");}
export function derive(records){
  const by=new Map(records.filter(r=>r.id).map(r=>[r.id,r]));
  const par=r=>(r.parents||[]).filter(p=>by.has(p));
  const orph=new Set();
  for(const r of by.values()) for(const p of r.parents||[]) if(!by.has(p)) orph.add(r.id);
  const col=new Map(),cyc=new Set();
  const dfs=(id,path)=>{const st=col.get(id)||0;
    if(st===1){cyc.add(path.slice(path.indexOf(id)).join(","));return;}
    if(st===2)return; col.set(id,1);
    for(const p of par(by.get(id))) dfs(p,[...path,id]); col.set(id,2);};
  for(const id of by.keys()) if(!col.get(id)) dfs(id,[]);
  for(const c of cyc) for(const x of c.split(",")) orph.add(x);
  const ap=new Map([...by].filter(([k])=>!orph.has(k)));
  const anc=id=>{const s=new Set(),st=[id];
    while(st.length){const x=st.pop();if(s.has(x))continue;s.add(x);
      for(const p of par(by.get(x))) st.push(p);} s.delete(id); return s;};
  const indeg=new Map([...ap.keys()].map(k=>[k,0]));
  for(const[k,r]of ap) indeg.set(k,par(r).filter(p=>ap.has(p)).length);
  const pool=new Set(ap.keys()), order=[];
  while(true){const ready=[...pool].filter(k=>indeg.get(k)===0).sort(cmp);
    if(!ready.length)break; const pick=ready[0];
    order.push(pick); pool.delete(pick);
    for(const k of pool) if(par(ap.get(k)).includes(pick)) indeg.set(k,indeg.get(k)-1);}
  const A=new Map([...ap.keys()].map(k=>[k,anc(k)]));
  const st=new Map(), se=new Map(), cf=new Map();
  for(const id of order){ const r=ap.get(id);
    for(const[key,v]of Object.entries(r.effects||{})){
      if(!st.has(key)){st.set(key,v);se.set(key,id);continue;}
      const cur=se.get(key);
      // v0.4.6 SPEC FIX: class is compared FIRST; ancestry is a tie-break
      // within an equal class. The previous line read `A.get(id).has(cur)
      // || rank >` -- ancestry consulted before class -- which the spec's
      // 5.1 now states explicitly is wrong. Changed so the probe set
      // demonstrates the fix rather than restating the bug.
      const rk=RANK[r.kind]||0, ck=RANK[ap.get(cur).kind]||0;
      const takes = rk>ck || (rk===ck && A.get(id).has(cur));
      const c=cf.get(key)||{key,winner:cur,losers:[]};
      if(takes){ if(canon(st.get(key))!==canon(v)&&!c.losers.includes(cur)) c.losers.push(cur);
        c.winner=id; st.set(key,v); se.set(key,id); }
      else if(!c.losers.includes(id)) c.losers.push(id);
      cf.set(key,c); } }
  const conflicts=[];
  for(const c of cf.values()){
    const vals=new Set(order.map(id=>canon((ap.get(id).effects||{})[c.key]))
      .filter(x=>x!=="undefined"));
    const wv=canon(st.get(c.key));
    const los=c.losers.filter(l=>canon((ap.get(l).effects||{})[c.key])!==wv);
    if(vals.size<2||!los.length) continue;
    conflicts.push({key:c.key,winner:c.winner,losers:los});}
  return {state:Object.fromEntries(st),order,conflicts,orphans:[...orph].sort(cmp)};
}
if(process.argv[2]==="--cli"){let t="";
  process.stdin.on("data",c=>t+=c).on("end",()=>console.log(JSON.stringify(derive(JSON.parse(t)))));}
