#include <algebraLib/variable_operators.hpp>
#include "traceConsistency.hpp"




	TraceConsistency::TraceConsistency(ProtoboardPtr pb,
		const ALUOutput& aluOutput,
		const FollowingTraceVariables& followingTraceVariables) :
		Gadget(pb), aluOutput_(aluOutput), followingTraceVariables_(followingTraceVariables),
		program_("program",
		std::dynamic_pointer_cast<const TinyRAMProtoboardParams>(pb_->params())->numRegisters(),
		std::dynamic_pointer_cast<const TinyRAMProtoboardParams>(pb_->params())->registerLength()){}


	void TraceConsistency::timeStampConsistency(){
		const Algebra::FElem generator = Algebra::FElem(getGF2E_X());
		// timestamp constraint
		Algebra::Variable timeStamp0 = followingTraceVariables_.first_.timeStamp_;
		Algebra::Variable timeStamp1 = followingTraceVariables_.second_.timeStamp_;
		CircuitPolynomial timeStampPoly(timeStamp1 + timeStamp0 * generator);
		pb_->addGeneralConstraint(timeStampPoly, "timestamp1 + timestamp0 * x", Opcode::NONE);
	}

	void TraceConsistency::pcConsistency(){
		const Algebra::FElem generator = Algebra::FElem(getGF2E_X());
		const int pcLength = program_.pcLength();
		Algebra::FElem x_i = Algebra::one();
		//// Calculate pc as packed sum_{i=0}^{i=registerLength} pc[i] * x_i
		//CircuitPolynomial pcPacked(followingTraceVariables_.second_.pc_[0] * x_i);
		//CircuitPolynomial prevPcPacked(followingTraceVariables_.first_.pc_[0] * x_i);
		//
		//for (int i = 1; i < pcLength; i++){
		//	x_i *= generator;
		//	pcPacked = pcPacked + (followingTraceVariables_.second_.pc_[i] * x_i);
		//	prevPcPacked = prevPcPacked + (followingTraceVariables_.first_.pc_[i] * x_i);
		//}


		//CircuitPolynomial pcPoly;
		//for (int i = 0; i < program_.size(); i++){
		//	CircuitPolynomial selector_i = getSelector(program_.size(), i, followingTraceVariables_.first_.pc_);
		//	Opcode opcode = program_.code()[i].opcode_;
		//	if (opcode == Opcode::JMP){
		//		pcPoly = pcPoly + ((pcPacked + aluOutput_.result_) * selector_i);
		//	}
		//	else if (opcode == Opcode::ANSWER){
		//		pcPoly = pcPoly + ((pcPacked + prevPcPacked) * selector_i);
		//	}
		//	else if (opcode == Opcode::CJMP){
		//		Algebra::FElem integerAsFElem = mapIntegerToFieldElement(0, pcLength, i + 1);
		//		Variable flag = followingTraceVariables_.first_.flag_;
		//		pcPoly = pcPoly + selector_i*(flag * (pcPacked + aluOutput_.result_) +
		//			(Algebra::one() + flag) * (pcPacked + integerAsFElem));
		//	}
		//	else if (opcode == Opcode::CNJMP){
		//		Algebra::FElem integerAsFElem = mapIntegerToFieldElement(0, pcLength, i + 1);
		//		Variable flag = followingTraceVariables_.first_.flag_;
		//		pcPoly = pcPoly + selector_i*((Algebra::one() + flag)  * (pcPacked + aluOutput_.result_) +
		//										flag * (pcPacked + integerAsFElem));
		//	}
		//	else{
		//		Algebra::FElem integerAsFElem = mapIntegerToFieldElement(0, pcLength, i + 1);
		//		pcPoly = pcPoly + ((pcPacked + integerAsFElem) *selector_i);
		//	}

		//}


		//*******************
		// Calculate pc as packed sum_{i=0}^{i=registerLength} pc[i] * x_i
		LinearCombination pcPacked(followingTraceVariables_.second_.pc_[0] * x_i);
		LinearCombination prevPcPacked(followingTraceVariables_.first_.pc_[0] * x_i);


		for (int i = 1; i < pcLength; i++){
			x_i *= generator;
			pcPacked = pcPacked + (followingTraceVariables_.second_.pc_[i] * x_i);
			prevPcPacked = prevPcPacked + (followingTraceVariables_.first_.pc_[i] * x_i);
		}
		//CircuitPolynomial pcPacked(pcPackedLC);
		//CircuitPolynomial prevPcPacked(prevPcPackedLC);

		vector<long> selectorToConstraint(program_.size());
		vector<Variable> selectorVariableVector = getPCVars(followingTraceVariables_.first_.pc_);
		//the vector that will contain the different relevant constraints according to the opcode
		vector<CircuitPolynomial> constraints;
		constraints.push_back(pcPacked + aluOutput_.result_);
		constraints.push_back(pcPacked + prevPcPacked);
		CircuitPolynomial pcPoly;
		for (unsigned int i = 0; i < program_.size(); i++){
			CircuitPolynomial selector_i = getSelector(program_.size(), i, followingTraceVariables_.first_.pc_);
			Opcode opcode = program_.code()[i].opcode_;
			if (opcode == Opcode::JMP){
				//pcPoly = pcPoly + ((pcPacked + aluOutput_.result_) * selector_i);
				selectorToConstraint[i] = 0;
			}
			else if (opcode == Opcode::ANSWER){
				//pcPoly = pcPoly + ((pcPacked + prevPcPacked) * selector_i);
				selectorToConstraint[i] = 1;
			}
			else if (opcode == Opcode::CJMP){
				Algebra::FElem integerAsFElem = mapIntegerToFieldElement(0, pcLength, i + 1);
				Variable flag = followingTraceVariables_.first_.flag_;
				//pcPoly = pcPoly + selector_i*(flag * (pcPacked + aluOutput_.result_) +
				//	(Algebra::one() + flag) * (pcPacked + integerAsFElem));
				constraints.push_back((flag * (pcPacked + aluOutput_.result_) +
					(Algebra::one() + flag) * (pcPacked + integerAsFElem)));
				selectorToConstraint[i] = constraints.size() - 1;

			}
			else if (opcode == Opcode::CNJMP){
				Algebra::FElem integerAsFElem = mapIntegerToFieldElement(0, pcLength, i + 1);
				Variable flag = followingTraceVariables_.first_.flag_;
				//pcPoly = pcPoly + selector_i*((Algebra::one() + flag)  * (pcPacked + aluOutput_.result_) +
				//	flag * (pcPacked + integerAsFElem));
				constraints.push_back((Algebra::one() + flag)  * (pcPacked + aluOutput_.result_) +
					flag * (pcPacked + integerAsFElem));
				selectorToConstraint[i] = constraints.size() - 1;

			}
			else{
				Algebra::FElem integerAsFElem = mapIntegerToFieldElement(0, pcLength, i + 1);
				//pcPoly = pcPoly + ((pcPacked + integerAsFElem) *selector_i);
				constraints.push_back((pcPacked + integerAsFElem));
				selectorToConstraint[i] = constraints.size() - 1;
			}

		}


		SelectorSum constraintPoly(constraints, selectorVariableVector, selectorToConstraint);
		pb_->addGeneralConstraint(constraintPoly, "sum_{i=0}^{programLength} (selector(i) * (pcAsFElem + (i+1)))", Opcode::NONE);
	}

	void TraceConsistency::registerConsistency(){
		unsigned int numRegistersTrace1 = followingTraceVariables_.first_.registers_.size();
		unsigned int numRegistersTrace2 = followingTraceVariables_.second_.registers_.size();
		GADGETLIB_ASSERT(numRegistersTrace1 == numRegistersTrace2, "TraceConsistency: number of registers should be the same");
		

		
		for (unsigned int i = 0; i < numRegistersTrace1; ++i){
			Variable regiSecond = followingTraceVariables_.second_.registers_[i];
			Variable regiFirst = followingTraceVariables_.first_.registers_[i];

			vector<long> selectorToConstraint(program_.size());
			vector<Variable> selectorVariableVector = getPCVars(followingTraceVariables_.first_.pc_);
			//preparing the vector that will contain the different relevant constraints according to the opcode
			vector<CircuitPolynomial> constraints;
			
			
			constraints.push_back(regiSecond + aluOutput_.result_);
			constraints.push_back(regiSecond + regiFirst);
			constraints.push_back(regiSecond + aluOutput_.value_);

			for (unsigned int j = 0; j < program_.size(); j++){
				Opcode opcode = program_.code()[j].opcode_;
				unsigned int dest = program_.code()[j].destIdx_;
				if (dest == i){
					switch (opcode)
					{
					case Opcode::ADD:
					case Opcode::SUB:
					case Opcode::AND:
					case Opcode::OR:
					case Opcode::XOR:
					case Opcode::MOV:
					case Opcode::SHL:
					case Opcode::SHR:
					case Opcode::MULL:
					case Opcode::RESERVED_OPCODE_24:
						selectorToConstraint[j] = 0;
						//constraintPoly = constraintPoly + (selector_j * (regiSecond + aluOutput_.result_));
						break;
					case Opcode::JMP:
					case Opcode::CJMP:
					case Opcode::CNJMP:
					case Opcode::CMPE:
					case Opcode::CMPA:
					case Opcode::CMPAE:
					case Opcode::CMPG:
					case Opcode::CMPGE:
					case Opcode::STOREW:
					case Opcode::ANSWER:
						selectorToConstraint[j] = 1;
						//constraintPoly = constraintPoly + (selector_j * (regiSecond + regiFirst));
						break;
					case Opcode::LOADW:
						selectorToConstraint[j] = 2;
						//constraintPoly = constraintPoly + (selector_j * (regiSecond + aluOutput_.value_));
						break;
					default:
						GADGETLIB_FATAL("Trace Consistency: No Such opcode exists");
						break;
					}
				}
				else{
					selectorToConstraint[j] = 1;
					//constraintPoly = constraintPoly + (selector_j * (regiSecond + regiFirst));
				}
			}
			SelectorSum constraintPoly(constraints, selectorVariableVector, selectorToConstraint);
			pb_->addGeneralConstraint(constraintPoly, "sum_{i=0}^{programLength} (selector(i) * (R'_j - R_j_OR_ALUres))", Opcode::NONE);
		}

		//unsigned int numRegistersTrace1 = followingTraceVariables_.first_.registers_.size();
		//unsigned int numRegistersTrace2 = followingTraceVariables_.second_.registers_.size();
		//GADGETLIB_ASSERT(numRegistersTrace1 == numRegistersTrace2, "TraceConsistency: number of registers should be the same");
		//for (int i = 0; i < numRegistersTrace1; ++i){
		//	CircuitPolynomial constraintPoly;
		//	for (int j = 0; j < program_.size(); j++){
		//		CircuitPolynomial selector_j = getSelector(program_.size(), j, followingTraceVariables_.first_.pc_);
		//		Opcode opcode = program_.code()[j].opcode_;
		//		Variable regiSecond = followingTraceVariables_.second_.registers_[i];
		//		Variable regiFirst = followingTraceVariables_.first_.registers_[i];
		//		unsigned int dest = program_.code()[j].destIdx_;
		//		if (dest == i){
		//			switch (opcode)
		//			{
		//			case Opcode::ADD:
		//			case Opcode::SUB:
		//			case Opcode::AND:
		//			case Opcode::OR:
		//			case Opcode::XOR:
		//			case Opcode::MOV:
		//			case Opcode::SHL:
		//			case Opcode::SHR:
		//				constraintPoly = constraintPoly + (selector_j * (regiSecond + aluOutput_.result_));
		//				break;
		//			case Opcode::JMP:
		//			case Opcode::CJMP:
		//			case Opcode::CNJMP:
		//			case Opcode::CMPE:
		//			case Opcode::CMPA:
		//			case Opcode::CMPAE:
		//			case Opcode::CMPG:
		//			case Opcode::CMPGE:
		//			case Opcode::STOREW:
		//			case Opcode::ANSWER:
		//				constraintPoly = constraintPoly + (selector_j * (regiSecond + regiFirst));
		//				break;
		//			case Opcode::LOADW:
		//				constraintPoly = constraintPoly + (selector_j * (regiSecond + aluOutput_.value_));
		//				break;
		//			default:
		//				GADGETLIB_FATAL("Trace Consistency: No Such opcode exists");
		//				break;
		//			}
		//		}
		//		else{
		//			constraintPoly = constraintPoly + (selector_j * (regiSecond + regiFirst));
		//		}
		//	}
		//	pb_->addGeneralConstraint(constraintPoly, "sum_{i=0}^{programLength} (selector(i) * (R'_j - R_j_OR_ALUres))", Opcode::NONE);
		//}

	}

	void TraceConsistency::timeStampWitness(){
		const Algebra::FElem generator = Algebra::FElem(getGF2E_X());
		Algebra::Variable timeStamp0 = followingTraceVariables_.first_.timeStamp_;
		Algebra::Variable timeStamp1 = followingTraceVariables_.second_.timeStamp_;
		pb_->val(timeStamp1) = pb_->val(timeStamp0) * generator;
		// Timestamp is represented by g^i in order to know the degree of the element
		// we keep a mapping between the FELem and i. Each time we increase the timestamp,
		// we update the mapping in protoboard
		unsigned int timeStampDegree = (pb_->val(timeStamp0) == Algebra::one()) ? 0 : pb_->getDegreeOfFElem(pb_->val(timeStamp0));
		pb_->addDegreeTranslation(pb_->val(timeStamp1), timeStampDegree + 1);

	}

	void TraceConsistency::pcWitness(unsigned int nextPC){
		Algebra::UnpackedWord pcUnpacked(followingTraceVariables_.second_.pc_);
		const int pcLength = program_.pcLength();
		GADGETLIB_ASSERT((int)pcUnpacked.size() == pcLength, "TraceConsistency: PC Length should be at least log2Ciel(programLength - 1) + 1");
		for (int i = 0; i < pcLength; ++i){
			pb_->val(pcUnpacked[i]) = (nextPC & 1U) ? Algebra::one() : Algebra::zero();
			nextPC >>= 1;
		}
	}

	void TraceConsistency::registersWitness(unsigned int programLine){
		unsigned int numRegistersTrace1 = followingTraceVariables_.first_.registers_.size();
		unsigned int numRegistersTrace2 = followingTraceVariables_.second_.registers_.size();
		GADGETLIB_ASSERT(numRegistersTrace1 == numRegistersTrace2, "TraceConsistency: number of registers should be the same");
		Opcode opcode = program_.code()[programLine].opcode_;
		unsigned int dest = program_.code()[programLine].destIdx_;
		for (unsigned int i = 0; i < numRegistersTrace1; i++){
			Variable regiSecond = followingTraceVariables_.second_.registers_[i];
			Variable regiFirst = followingTraceVariables_.first_.registers_[i];
			if (dest != i){
				pb_->val(regiSecond) = pb_->val(regiFirst);
			}
			else{
				switch (opcode)
				{
				case Opcode::ADD:
				case Opcode::SUB:
				case Opcode::AND:
				case Opcode::OR:
				case Opcode::XOR:
				case Opcode::SHL:
				case Opcode::SHR:
				case Opcode::MOV:
				case Opcode::MULL:
				case Opcode::RESERVED_OPCODE_24:
					pb_->val(regiSecond) = pb_->val(aluOutput_.result_);
					break;
				case Opcode::JMP:
				case Opcode::CJMP:
				case Opcode::CNJMP:
				case Opcode::CMPE:
				case Opcode::CMPA:
				case Opcode::CMPAE:
				case Opcode::CMPG:
				case Opcode::STOREW:
				case Opcode::ANSWER:
					pb_->val(regiSecond) = pb_->val(regiFirst);
					break;
				case Opcode::LOADW:
					pb_->val(regiSecond) = pb_->val(aluOutput_.value_);
					break;
				default:
					GADGETLIB_FATAL("Trace Consistency: No Such opcode exists");
					break;
				}
			}
		}
	}

	GadgetPtr TraceConsistency::create(ProtoboardPtr pb,
		const ALUOutput& aluOutput,
		const FollowingTraceVariables& followingTraceVariables){
		GadgetPtr pGadget(new TraceConsistency(pb, aluOutput, followingTraceVariables));
		pGadget->init();
		return pGadget;
	}


	void TraceConsistency::setProgram(const TinyRAMProgram& program){
		program_ = program;
	}

	void TraceConsistency::generateConstraints(){
		GADGETLIB_ASSERT(aluOutput_.flag_ == followingTraceVariables_.second_.flag_, "TraceConsistency: aluOutput.flag == followingTraceVariables_.second_.flag_");
		GADGETLIB_ASSERT(program_.size() > 0, "TraceConsistency: The program should be initialized");
		const Algebra::FElem generator = Algebra::FElem(getGF2E_X());
		::std::shared_ptr<const TinyRAMProtoboardParams> params = std::dynamic_pointer_cast<const TinyRAMProtoboardParams>(pb_->params());
		timeStampConsistency();
		pcConsistency();
		registerConsistency();

	}


	void TraceConsistency::generateWitness(unsigned int programLine){
		GADGETLIB_ASSERT(program_.size() > 0, "TraceConsistency: The program should be initialized");
		GADGETLIB_ASSERT(programLine < program_.size(), "TraceCosistency: programLine should be less than the program Length ");
		::std::shared_ptr<const TinyRAMProtoboardParams> params = std::dynamic_pointer_cast<const TinyRAMProtoboardParams>(pb_->params());
		const Algebra::FElem generator = Algebra::FElem(getGF2E_X());
		timeStampWitness();
		Opcode opcode = program_.code()[programLine].opcode_;
		if (opcode == Opcode::JMP || opcode == Opcode::CJMP || opcode == Opcode::CNJMP){
			unsigned int nextLine;
			if (program_.code()[programLine].arg2isImmediate_){
				nextLine = program_.code()[programLine].arg2IdxOrImmediate_;
			}
			else{
				unsigned int regIndex = program_.code()[programLine].arg2IdxOrImmediate_;
				nextLine = mapFieldElementToInteger(0, program_.pcLength(), pb_->val(followingTraceVariables_.first_.registers_[regIndex]));
			}

			// Calculate PC
			if (opcode == Opcode::JMP){
				pcWitness(nextLine);
			}
			else if (opcode == Opcode::CJMP){
				if (pb_->val(followingTraceVariables_.first_.flag_) == Algebra::one()){
					pcWitness(nextLine);
				}
				else{
					pcWitness(programLine + 1);
				}
			}
			else{
				if (pb_->val(followingTraceVariables_.first_.flag_) == Algebra::zero()){
					pcWitness(nextLine);
				}
				else{ pcWitness(programLine + 1); }
			}
		}
		else if (opcode == Opcode::ANSWER){
			pcWitness(programLine);
		}
		else{
			pcWitness(programLine + 1); 
		}
		registersWitness(programLine);

	}


