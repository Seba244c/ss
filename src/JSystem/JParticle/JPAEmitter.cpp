//
// Generated By: dol2asm
// Translation Unit: JPAEmitter
//

#include "JSystem/JParticle/JPAEmitter.h"
#include "JSystem/JParticle/JPAEmitterManager.h"
#include "JSystem/JParticle/JPAParticle.h"
#include "JSystem/JParticle/JPAResourceManager.h"
#include "JSystem/JParticle/JPABaseShape.h"
#include "egg/math/eggVector.h"

//
// Types:
//

//
// Forward References:
//

//
// External References:
//

//
// Declarations:
//

/* 8027E6A4-8027E6EC 278FE4 0048+00 0/0 14/14 16/16 .text            __dt__18JPAEmitterCallBackFv */
JPAEmitterCallBack::~JPAEmitterCallBack() {
}

/* 8027E6EC-8027EA40 27902C 0354+00 0/0 1/1 0/0 .text
 * init__14JPABaseEmitterFP17JPAEmitterManagerP11JPAResource    */
void JPABaseEmitter::init(JPAEmitterManager* param_0, JPAResource* param_1) {
    mpEmtrMgr = param_0;
    mpRes = param_1;
    mpRes->getDyn()->getEmitterScl(&mLocalScl);
    mpRes->getDyn()->getEmitterTrs(&mLocalTrs);
    mpRes->getDyn()->getEmitterDir(&mLocalDir);
    mLocalDir.normalise();
    mpRes->getDyn()->getEmitterRot(&mLocalRot);
    mMaxFrame = mpRes->getDyn()->getMaxFrame();
    mLifeTime = mpRes->getDyn()->getLifetime();
    mVolumeSize = mpRes->getDyn()->getVolumeSize();
    mRate = mpRes->getDyn()->getRate();
    mRateStep = mpRes->getDyn()->getRateStep();
    mVolumeSweep = mpRes->getDyn()->getVolumeSweep();
    mVolumeMinRad = mpRes->getDyn()->getVolumeMinRad();
    mAwayFromCenterSpeed = mpRes->getDyn()->getInitVelOmni();
    mAwayFromAxisSpeed = mpRes->getDyn()->getInitVelAxis();
    mDirSpeed = mpRes->getDyn()->getInitVelDir();
    mSpread = mpRes->getDyn()->getInitVelDirSp();
    mRndmDirSpeed = mpRes->getDyn()->getInitVelRndm();
    mAirResist = mpRes->getDyn()->getAirRes();
    mRndm.set_seed(mpEmtrMgr->pWd->mRndm.get_rndm_u());
    MTXIdentity(mGlobalRot);
    mGlobalScl.set(1.0f, 1.0f, 1.0f);
    mGlobalTrs.setZero();
    mGlobalPScl.set(1.0f, 1.0f);
    mGlobalEnvClr.a = 0xff;
    mGlobalEnvClr.b = 0xff;
    mGlobalEnvClr.g = 0xff;
    mGlobalEnvClr.r = 0xff;
    mGlobalPrmClr.a = 0xff;
    mGlobalPrmClr.b = 0xff;
    mGlobalPrmClr.g = 0xff;
    mGlobalPrmClr.r = 0xff;
    param_1->getBsp()->getPrmClr(&mPrmClr);
    param_1->getBsp()->getEnvClr(&mEnvClr);
    mpUserWork = NULL;
    mScaleOut = 1.0f;
    mEmitCount = 0.0f;
    initStatus(0x30);
    mDrawTimes = 1;
    mTick = 0;
    mWaitTime = 0;
    mRateStepTimer = 0;
    mTexAnmIdx = 0;
}

/* 8027EA40-8027EB60 279380 0120+00 0/0 3/3 0/0 .text            createParticle__14JPABaseEmitterFv
 */
JPABaseParticle* JPABaseEmitter::createParticle() {
    if (mpPtclPool->getNum() != 0) {
        JPANode<JPABaseParticle>* node = mpPtclPool->pop_front();
        mAlivePtclBase.push_front(node);
        mpRes->getDyn()->calc(mpEmtrMgr->pWd);
        node->mData.init_p(mpEmtrMgr->pWd);
        return &node->mData;
    }

    return NULL;
}

/* 8027EB60-8027EC60 2794A0 0100+00 0/0 1/1 0/0 .text
 * createChild__14JPABaseEmitterFP15JPABaseParticle             */
JPABaseParticle* JPABaseEmitter::createChild(JPABaseParticle* parent) {
    if (mpPtclPool->getNum() != 0) {
        JPANode<JPABaseParticle>* node = mpPtclPool->pop_front();
        mAlivePtclChld.push_front(node);
        node->mData.init_c(mpEmtrMgr->pWd, parent);
        return &node->mData;
    }

    return NULL;
}

/* 8027EC60-8027EDD4 2795A0 0174+00 0/0 2/2 21/21 .text deleteAllParticle__14JPABaseEmitterFv */
void JPABaseEmitter::deleteAllParticle() {
    while (mAlivePtclBase.getNum())
        mpPtclPool->push_front(mAlivePtclBase.pop_back());
    while (mAlivePtclChld.getNum())
        mpPtclPool->push_front(mAlivePtclChld.pop_back());
}

/* 8027EDD4-8027EE14 279714 0040+00 0/0 1/1 0/0 .text processTillStartFrame__14JPABaseEmitterFv */
bool JPABaseEmitter::processTillStartFrame() {
    JPADynamicsBlock* dyn = mpRes->getDyn();
    s16 startFrame = dyn->getStartFrame();

    if (mWaitTime >= startFrame)
        return true;

    if (!(mStatus & 2))
        mWaitTime++;

    return false;
}

/* 8027EE14-8027EEB0 279754 009C+00 0/0 1/1 0/0 .text processTermination__14JPABaseEmitterFv */
bool JPABaseEmitter::processTermination() {
    if (checkStatus(0x100)) {
        return true;
    }

    if (mMaxFrame == 0) {
        return false;
    }
    if (mMaxFrame < 0) {
        setStatus(8);
        return getParticleNumber() == 0;
    } 
    if (mTick >= mMaxFrame) {
        setStatus(8);
        if (checkStatus(0x40)) {
            return false;
        }
        return getParticleNumber() == 0;
    }
    return false;
}

/* 8027EEB0-8027EF30 2797F0 0080+00 0/0 1/1 0/0 .text
 * calcEmitterGlobalPosition__14JPABaseEmitterCFPQ29JGeometry8TVec3<f> */
void JPABaseEmitter::calcEmitterGlobalPosition(EGG::Vector3f* dst) const {
    Mtx mtx;
    MTXScale(mtx, mGlobalScl.x, mGlobalScl.y, mGlobalScl.z);
    MTXConcat(mGlobalRot, mtx, mtx);
    mtx[0][3] = mGlobalTrs.x;
    mtx[1][3] = mGlobalTrs.y;
    mtx[2][3] = mGlobalTrs.z;
    MTXMultVec(mtx, mLocalTrs, *dst);
}

/* 8027EF30-8027EF40 279870 0010+00 0/0 1/1 0/0 .text getCurrentCreateNumber__14JPABaseEmitterCFv
 */
u32 JPABaseEmitter::getCurrentCreateNumber() const {
    return mpEmtrMgr->pWd->mEmitCount;
}

/* 8027EF40-8027EF50 279880 0010+00 0/0 3/3 0/0 .text            getDrawCount__14JPABaseEmitterCFv
 */
u8 JPABaseEmitter::getDrawCount() const {
    return mpEmtrMgr->pWd->mDrawCount;
}

/* 8027EF50-8027EFA4 279890 0054+00 0/0 1/1 0/0 .text
 * loadTexture__14JPABaseEmitterFUc11_GXTexMapID                */
bool JPABaseEmitter::loadTexture(u8 idx, GXTexMapID texMapID) {
    mpEmtrMgr->pWd->mpResMgr->load(mpRes->getTexIdx(idx), texMapID);
    return true;
}
