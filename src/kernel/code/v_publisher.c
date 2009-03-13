
#include "v__publisher.h"
#include "v__publisherQos.h"
#include "v_participant.h"
#include "v__domain.h"
#include "v__domainAdmin.h"
#include "v__writer.h"
#include "v_group.h"
#include "v_observable.h"
#include "v__observer.h"
#include "v_public.h"
#include "v__policy.h"
#include "v_event.h"
#include "v_proxy.h"
#include "v_time.h"

#include "q_expr.h"

#include "os_report.h"
#include "os_heap.h"


/**************************************************************
 * Private functions
 **************************************************************/
static c_bool
publish(
    c_object o,
    c_voidp arg)
{
    v_domain d = (v_domain)o;
    v_writer w = (v_writer)arg;

    return v_writerPublish(w,d);
}

static c_bool
unpublish(
    c_object o,
    c_voidp arg)
{
    v_domain d = (v_domain)o;
    v_writer w = (v_writer)arg;

    return v_writerUnPublish(w,d);
}

static c_bool
qosChangedAction(
    c_object o,
    c_voidp arg)
{
    v_writer w = v_writer(o);

    v_writerNotifyChangedQos(w, (v_writerNotifyChangedQosArg *)arg);

    return TRUE;
}

/**************************************************************
 * constructor/destructor
 **************************************************************/
v_publisher
v_publisherNew(
    v_participant participant,
    const c_char *name,
    v_publisherQos qos,
    c_bool enable)
{
    v_kernel kernel;
    v_publisher p;
    v_publisherQos q;

    assert(C_TYPECHECK(participant,v_participant));

    kernel = v_objectKernel(participant);

    q = v_publisherQosNew(kernel,qos);
    if (q != NULL) {
        p = v_publisher(v_objectNew(kernel,K_PUBLISHER));
        v_observerInit(v_observer(p),name,NULL,enable);
        p->domains     = v_domainAdminNew(kernel);
        p->writers     = c_setNew(v_kernelType(kernel,K_WRITER));
        p->qos         = q;
        p->suspendTime = C_TIME_INFINITE;
        p->participant = participant;
        c_lockInit(&p->lock,SHARED_LOCK);
        v_participantAdd(participant,v_entity(p));
        assert(c_refCount(p) == 3);  /* as handle, by participant and the local variable p */
        if (enable) {
            v_publisherEnable(p);
        }
    } else {
        OS_REPORT(OS_ERROR, "v_publisherNew", 0,
                  "Publisher not created: inconsistent qos");
        p = NULL;
    }
    return p;
}

v_result
v_publisherEnable(
    v_publisher _this)
{
    v_kernel kernel;
    c_iter list;
    c_char *partitionName;
    v_result result = V_RESULT_ILL_PARAM;

    if (_this) {
        kernel = v_objectKernel(_this);
        v_observableAddObserver(v_observable(kernel->groupSet), v_observer(_this), NULL);

        if (_this->qos->partition != NULL) {
            list = v_partitionPolicySplit(_this->qos->partition);
            while((partitionName = c_iterTakeFirst(list)) != NULL) {
                v_publisherPublish(_this,partitionName);
                os_free(partitionName);
            }
            c_iterFree(list);
        }
        result = V_RESULT_OK;
    }
    return result;
}

void
v_publisherFree(
    v_publisher p)
{
    v_writer o;
    v_participant participant;
    v_kernel kernel;

    assert(C_TYPECHECK(p,v_publisher));

    kernel = v_objectKernel(p);
    v_observableRemoveObserver(v_observable(kernel->groupSet),v_observer(p));

    while ((o = c_take(p->writers)) != NULL) {
        /* remove writer from all partitions */
        v_writerFree(o);
        c_free(o);
    }
    participant = v_participant(p->participant);
    if (participant != NULL) {
        v_participantRemove(participant,v_entity(p));
        p->participant = NULL;
    }
    v_observerFree(v_observer(p));
}

void
v_publisherDeinit(
    v_publisher p)
{
    assert(C_TYPECHECK(p,v_publisher));

    v_domainAdminFree(p->domains);
    p->domains = NULL;
    v_observerDeinit(v_observer(p));
}

/**************************************************************
 * Protected functions
 **************************************************************/
v_publisherQos
v_publisherGetQosRef(
    v_publisher p)
{
    assert(p != NULL);
    assert(C_TYPECHECK(p,v_publisher));

    return p->qos;  /* the reference is read only! */
}


v_result
v_publisherSetQos(
    v_publisher p,
    v_publisherQos qos)
{
    v_result result;
    v_qosChangeMask cm;
    v_writerNotifyChangedQosArg arg;
    v_domain d;

    assert(p != NULL);
    assert(C_TYPECHECK(p,v_publisher));

    arg.addedDomains = NULL;
    arg.removedDomains = NULL;

    c_lockWrite(&p->lock);
    result = v_publisherQosSet(p->qos, qos, v_entity(p)->enabled,&cm);
    if ((result == V_RESULT_OK) && (cm != 0)) {
        if (cm & V_POLICY_BIT_PARTITION) { /* partition policy has changed! */            
            v_domainAdminSet(p->domains, p->qos->partition, &arg.addedDomains, &arg.removedDomains);
        }
        c_walk(p->writers, qosChangedAction, &arg);

        d = v_domain(c_iterTakeFirst(arg.addedDomains));
        while (d != NULL) {
            c_free(d);
            d = v_domain(c_iterTakeFirst(arg.addedDomains));
        }
        c_iterFree(arg.addedDomains);
        d = v_domain(c_iterTakeFirst(arg.removedDomains));
        while (d != NULL) {
            c_free(d);
            d = v_domain(c_iterTakeFirst(arg.removedDomains));
        }
        c_iterFree(arg.removedDomains);
    }
    c_lockUnlock(&p->lock);

    return result;
}


static c_bool
assertLivelinessWriter(
    c_object o,
    c_voidp arg)
{
    v_entity e = v_entity(o);

    if (v_objectKind(e) == K_WRITER) {
        if (arg == NULL) { /* assert ALL writers */
            v_writerAssertByPublisher(v_writer(e));
        } else { /* assert all writers except writer starting the event! */
            if (v_handleIsEqual(v_publicHandle(v_public(e)),
                                ((v_event)arg)->source) == FALSE) {
                v_writerAssertByPublisher(v_writer(e));
            }
        }
    }
    return TRUE;
}

void
v_publisherNotify(
    v_publisher p,
    v_event e)
{
    c_bool connect;
    v_group g;
    c_iter addedDomains;
    v_domain d;

    if (e->kind == V_EVENT_NEW_GROUP) {
        g = v_group(e->userData);
        connect = v_domainAdminFitsInterest(p->domains, g->partition);
        if (connect) {
            addedDomains = v_domainAdminAdd(p->domains,
                                            v_entityName(g->partition));
            d = v_domain(c_iterTakeFirst(addedDomains));
            while (d != NULL) {
                c_free(d);
                d = v_domain(c_iterTakeFirst(addedDomains));
            }
            c_iterFree(addedDomains);
            c_walk(p->writers, (c_action)v_writerPublishGroup, g);
        }
    }
    if (e->kind == V_EVENT_LIVELINESS_ASSERT) {
        c_lockRead(&p->lock);
        c_walk(p->writers, assertLivelinessWriter, (c_voidp)e);
        c_lockUnlock(&p->lock);
    }
}

/**************************************************************
 * Public functions
 **************************************************************/
static c_bool
collectDomains(
    c_object o,
    c_voidp arg)
{
    v_domain d = (v_domain)o;
    c_iter iter = (c_iter)arg;

    iter = c_iterInsert(iter,c_keep(d));
    return TRUE;
}

void
v_publisherAddWriter(
    v_publisher p,
    v_writer w)
{
    v_domain d;
    c_iter iter;

    assert(p != NULL);
    assert(C_TYPECHECK(p,v_publisher));
    assert(w != NULL);
    assert(C_TYPECHECK(w,v_writer));

    iter = c_iterNew(NULL);
    v_domainAdminWalkDomains(p->domains, collectDomains, iter);
    while ((d = c_iterTakeFirst(iter)) != NULL) {
        v_writerPublish(w,d);
        c_free(d);
    }
    c_iterFree(iter);
    c_lockWrite(&p->lock);
    c_setInsert(p->writers,w);
    c_lockUnlock(&p->lock);
}

void
v_publisherRemoveWriter(
    v_publisher p,
    v_writer w)
{
    v_writer found;

    assert(p != NULL);
    assert(C_TYPECHECK(p,v_publisher));
    assert(w != NULL);
    assert(C_TYPECHECK(w,v_writer));

    c_lockWrite(&p->lock);
    found = c_remove(p->writers,w,NULL,NULL);
    c_lockUnlock(&p->lock);
    c_free(found);
}

c_bool
v_publisherCheckDomainInterest(
    v_publisher p,
    v_domain partition)
{
    return v_domainAdminFitsInterest(p->domains, partition);
}    

void
v_publisherPublish(
    v_publisher p,
    const c_char *partitionExpr)
{
    v_domain d;
    v_writerNotifyChangedQosArg arg;
    v_partitionPolicy old;

    assert(p != NULL);
    assert(C_TYPECHECK(p,v_publisher));    

    arg.removedDomains = NULL;

    arg.addedDomains = v_domainAdminAdd(p->domains, partitionExpr);
    c_lockWrite(&p->lock);
    old = p->qos->partition;
    p->qos->partition = v_partitionPolicyAdd(old, partitionExpr, c_getBase(c_object(p)));
    c_free(old);

    c_walk(p->writers, qosChangedAction, &arg);

    d = v_domain(c_iterTakeFirst(arg.addedDomains));
    while (d != NULL) {
        c_free(d);
        d = v_domain(c_iterTakeFirst(arg.addedDomains));
    }
    c_iterFree(arg.addedDomains);

    c_lockUnlock(&p->lock);
}

void
v_publisherUnPublish(
    v_publisher p,
    const c_char *partitionExpr)
{
    v_domain d;
    v_writerNotifyChangedQosArg arg;
    v_partitionPolicy old;

    assert(p != NULL);
    assert(C_TYPECHECK(p,v_publisher));

    arg.addedDomains = NULL;

    arg.removedDomains = v_domainAdminRemove(p->domains, partitionExpr);
    c_lockWrite(&p->lock);
    old = p->qos->partition;
    p->qos->partition = v_partitionPolicyRemove(old, partitionExpr, c_getBase(c_object(p)));
    c_free(old);

    c_walk(p->writers, qosChangedAction, &arg);

    d = v_domain(c_iterTakeFirst(arg.removedDomains));
    while (d != NULL) {
        c_free(d);
        d = v_domain(c_iterTakeFirst(arg.removedDomains));
    }
    c_iterFree(arg.removedDomains);

    c_lockUnlock(&p->lock);
}


c_iter
v_publisherLookupWriters(
    v_publisher p,
    const c_char *topicExpr)
{
    c_iter list;
    c_collection q;
    q_expr expr;
    c_value params[1];

    assert(p != NULL);
    assert(C_TYPECHECK(p,v_publisher));

    expr = (q_expr)q_parse("topic.name like %0");
    params[0] = c_stringValue((c_char *)topicExpr);
    q = c_queryNew(p->writers,expr,params);
    q_dispose(expr);

    c_lockRead(&p->lock);
    list = c_select(q,0);
    c_lockUnlock(&p->lock);
    c_free(q);
    return list;
}

c_iter
v_publisherLookupDomains(
    v_publisher p,
    const c_char *partitionExpr)
{
    c_iter list;

    assert(p != NULL);
    assert(C_TYPECHECK(p,v_publisher));

    c_lockRead(&p->lock);
    list = v_domainAdminLookupDomains(p->domains, partitionExpr);
    c_lockUnlock(&p->lock);

    return list;
}

void
v_publisherSuspend (
    v_publisher p)
{
    assert(p != NULL);
    assert(C_TYPECHECK(p,v_publisher));

    c_lockWrite(&p->lock);
    if (c_timeCompare(p->suspendTime, C_TIME_INFINITE) == C_EQ) {
        p->suspendTime = v_timeGet();
    } /* else publisher was already suspended, so no-op */
    c_lockUnlock(&p->lock);
}

c_bool
v_publisherResume (
    v_publisher p)
{
    c_iter writers;
    v_writer w;
    c_time suspendTime;
    c_bool resumed = FALSE;
    
    assert(p != NULL);
    assert(C_TYPECHECK(p,v_publisher));

    c_lockWrite(&p->lock);
    if (c_timeCompare(p->suspendTime, C_TIME_INFINITE) != C_EQ) {    
        suspendTime = p->suspendTime;
        p->suspendTime = C_TIME_INFINITE;
        writers = c_select(p->writers, 0);
        c_lockUnlock(&p->lock);
    
        w = v_writer(c_iterTakeFirst(writers));
        while (w != NULL) {
            v_writerResumePublication(w, &suspendTime);
            c_free(w);
            w = v_writer(c_iterTakeFirst(writers));
        }
        c_iterFree(writers);
        resumed = TRUE;
    } else {
        c_lockUnlock(&p->lock);
    }
    
    return resumed;
}

void
v_publisherCoherentBegin (
    v_publisher p)
{
    assert(p != NULL);
    assert(C_TYPECHECK(p,v_publisher));

}

void
v_publisherCoherentEnd (
    v_publisher p)
{
    assert(p != NULL);
    assert(C_TYPECHECK(p,v_publisher));
}

v_publisherQos
v_publisherGetQos(
    v_publisher p)
{
    v_publisherQos qos;

    assert(p != NULL);
    assert(C_TYPECHECK(p,v_publisher));

    c_lockRead(&p->lock);
    qos = v_publisherQosNew(v_objectKernel(p), p->qos);
    c_lockUnlock(&p->lock);

    return qos;
}